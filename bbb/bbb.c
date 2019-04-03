// Mark Gomes
// Big Bad Bowsers
// CSC 460 - Dr. Allen
// 04/8/19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>

typedef enum{ false, true } bool;

typedef struct job
{
	int pid;
	int cputime;
	int start;	// seconds
	int end;

	job()
	{
		pid = -1;
		cputime = -1;
		start = -1;
		end = -1;
	}

	job(int _pid, int _cputime, int start, int end)
	{
		pid = _pid;
		cputime = _cputime;
		start = _start;
		end = _end;
	}
} job;

typedef struct global
{
	int timeslice;
	int jobnum;
	int killswitch;

	global()
	{
		timeslice = 1;
		jobnum = 0;
		killswitch = 0;	// 1 -> kill switch
	}
} globals;

void dispatch(/*int jobsem, int jobshm*/);
void simCPU(int ts/*int jobsem, int jobshm*/);
void systemTime(/*int timeshm*/);
void forkHelp(/*int *id*/);
void setupSystem(/*int *jobsem, int *timeshm, int *jobshm*/);

void p(int s, int sem_id);	// Wait
void v(int s, int sem_id);	// Signal
job jobcopy(job a);		// deep copy job

int jobsem, timeshm, jobshm, globshm, id;

int main(int argc, char** argv)
{
	srand(time(NULL)); 	// create random seed based on time
	//int jobsem, timeshm, jobshm, id;
	
	// set up shared memory for system
        setupSystem(&jobsem, &timeshm, &jobshm);

	// store timeslice in global shm
	int ts = 1;
	if(argc > 1)
		if(stoi(argv[1] > 0))
			ts = argv[1];

	printf("Let the Simulation Begin!! \n\n");
	// fork off helps to preform tasks based on id
	forkHelp(/*&id*/);
	switch(id)
	{
		case 0:
		dispatch(/*jobsem, jobshm*/);
		break;

		case 1:
		simCPU(ts/*jobsem, jobshm*/);
		break;

		case 2:
		systemTime(/*timeshm*/);
		break;

		default:
		break;
	}
}

void dispatch(/*int jobsem, int jobshm*/)
{
	// pointers to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	job* jobs = (job*) shmat(jobshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);
	job *temp;
	int i;

	while(globs->killswitch != 1)	// run until kill swithc is activated
	{
		// wait for cpu to execute a time slice
		p(5, jobsem);
		p(MUTEX, jobsem);	// block user access

		// cycle the queue
		jobcopy(temp, &jobs[0]);
		for(i = 0; i < globs->jobnum-1; i++)
		{
			jobcopy(&jobs[i], &jobs[i+1]);
		}

		// drop completed jobs
		if(temp.N != 0)
		{
			// place unfinished jobs at back of job queue
			jobs[globs->jobnum-1] = temp;
		}
		else	// free sem for open queue space
		{
			/*record info for completed job
			 *set end time
			 *mabye send signal to user to read completed job in job[0]***
			*/

			v(globs->jobnum-1, jobsem);
		}

		// signal to the CPU
		v(0, jobsem);
		v(MUTEX, jobsem);	// allow user access
	}

	v(FIN, jobsem);   // signal piece of clean up
}

void simCPU(int ts/*int jobsem, int jobshm*/)
{
	// pointers to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	job* jobs = (job*) shmat(jobshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);

	// declare timeslice in global shmem
	globs->timeslice = ts;

	int N, pid, timeslice, timer;

	while(globs->killswitch != 1)	// run until kill switch is activated
	{
		// check job queue for next job
		p(0, jobsem);		// wait for signal from user
		N = jobs[0].cputime;
		pid = jobs[0].pid;
		// confirm request
		printf("\t\tCPU RECIEVES request for %d from %d \n", N, pid);
		// execute N cputime
		if(globs->timeslice > N)
			timeslice = N;
		else
			timeslice = globs->timeslice;

		// sleep for timeslice based on system clock
		timer = *st + timeslice;
		while(*st < timer)
		{/*execute cpu time*/}
		jobs[0].end = *st;

		jobs[0].N = jobs[0].N - timeslice;
		// confirm job completion
		if(jobs[0].N == 0)
		{
			printf("\t\tCPU FINISHED request for %d from %d \n", N, pid);
		}
		// signal to dispatcher job is complete
		v(5, jobsem);
	}

	v(FIN, jobsem);   // signal piece of clean up
}

void systemTime(/*int timeshm*/)
{
	// pointers to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);

	// time -> miliseconds
	clock_t start = clock() / CLOCKS_PER_SEC;
	clock_t curr = start;
	
	*st = curr;
	int count = 1;
	while(globs->killswitch != 1)      // run until kill switch is activated
	{
		// update time every second
		if((curr - start) >= count * 1.0)
		{
			*st = count;	// convert to seconds
			printf("system time : %d \n", count);
			count++;
		}
		curr = clock() / CLOCKS_PER_SEC;
	}

	v(FIN, jobsem);	  // signal piece of clean up
}

void forkHelp(/*int *id*/)
{
	int a = 0;
        int i;
        for(i = 1; i < 3; i++)          //create 2 children from og parent
        {
                a = fork();

                if(a == 0)              //remove child
                {
                        id = i;                //assign ids 1-2
                        break;
                }
        }

        if( a != 0)
        {
                id = 0;                // assign id 0
        }
}

void setupSystem(int timeslice/*int *jobsem, int *timeshm, int *jobshm*/)
{
	// open file to hold shared memory id
	FILE *fopen(), *fp;
	
	if((fp = fopen("./share.txt","w")) == NULL)
        {
                printf("start:  could not open file.\n");
                exit(0);
        }

	// ask OS for semaphores
	jobsem = semget( IPC_PRIVATE, 5, 0777);
	
	// see if request is accepted
	if(jobsem == -1)
	{
		printf("%s", "SemGet Failed.\n");
		fclose(0);
		exit(0);
	}

	// initialize your sems
	semctl(jobsem, CPU, SETVAL, 0);		// CPU signaler
	//semctl(jobsem, 1, SETVAL, 1);
	//semctl(jobsem, 2, SETVAL, 0);
        semctl(jobsem, EMPTY, SETVAL, 0);	// user signalers
	semctl(jobsem, FULL, SETVAL, 0);
	semctl(jobsem, DPR, SETVAL, 0);		// Dispatcher signaler
	semctl(jobsem, FIN, SETVAL, 0);		// clean up signaler
	semctl(jobsem, MUTEX, SETVAL, 1);	// mutex
	
	// set up shared memory for job buffer
	jobshm = shmget(IPC_PRIVATE, 5*sizeof(struct job), 0777);
	if(jobshm == -1)
        {
                printf("ERROR - can't get shared memory \n");
		fclose(fp);
                exit(0);
        }

	// set up shared memory for system time
	timeshm = shmget(IPC_PRIVATE, sizeof(int), 0777);
        if(timeshm == -1)
        {
                printf("ERROR - can't get shared memory \n");
		fclose(0);
                exit(0);
        }

	// set up shared memory for globals
	globshm = shmget(IPC_PRIVATE, 5*sizeof(struct global), 0777);
        if(globnshm == -1)
        {
                printf("ERROR - can't get shared memory \n");
                fclose(fp);
                exit(0);
        }

	// print sem / shmem address to file
	fprintf(fp, "%d\n", jobsem);
	fprintf(fp, "%d\n", timeshm);
	fprintf(fp, "%d\n", jobshm);
	fprintf(fp, "%d\n", globshm);
	fclose(fp);
}

void p(int s, int sem_id)	// Wait
{
	struct sembuf sops;

	sops.sem_num = s;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1)
		printf("%s", "'P' error\n");
}

void v(int s, int sem_id)	// Signal
{
	struct sembuf sops;
	
	sops.sem_num = s;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1)
		printf("%s", "'V' error\n");
}

void jobcopy(job *a, job *b)	// copies b into a
{
	a->pid = b->pid;
	a->cputime = b->cputime;
	a->timeslice = b->timeslice;
	a->start = b->start;
	a->end = b->end;
}
