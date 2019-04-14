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
#define CPU 0
#define DPR 1
#define EMPTY 2
#define FULL 3
#define FIN 4
#define MUTEX 5
#define JCALL 6
#define JREPS 7

typedef struct job
{
	int pid;
	int cputime;
	int start;	// seconds
	int end;
} job;

typedef struct global
{
	int timeslice;
	int jobnum;
	int killswitch;
	int subjobs;
} global;

void dispatch();
void simCPU(int ts);
void systemTime();
void forkHelp();
void setupSystem();

void p(int s, int sem_id);	// Wait
void v(int s, int sem_id);	// Signal
void jobcopy(job *a, job *b);		// deep copy job

int jobsem, timeshm, jobshm, globshm, id;

int main(int argc, char** argv)
{
	srand(time(NULL)); 	// create random seed based on time
	
	// set up shared memory for system
        setupSystem(&jobsem, &timeshm, &jobshm);

	// store timeslice in global shm
	int ts = 1;
	if(argc > 1)
		if(atoi(argv[1]) > 0)
			ts = atoi(argv[1]);

	printf("Let the Simulation Begin!! \n\n");
	// fork off helps to preform tasks based on id
	forkHelp();
	switch(id)
	{
		case 0:
			dispatch();
			break;

		case 1:
			simCPU(ts);
			break;

		case 2:
			systemTime();
			break;

		default:
			break;
	}
}

void dispatch()
{
	//printf("Dispatcher is alive\n");

	// pointers to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	job* jobs = (job*) shmat(jobshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);
	job j;
	job *temp = &j;
	int i;

	while(globs->killswitch != 1)	// run until kill switch is activated
	{
		// wait for cpu to execute a time slice
		p(DPR, jobsem);		// wait for dispatcher signal
		//printf("***dp : received DPR signal\n");
		p(MUTEX, jobsem);	// mutex
		//printf("***dp : enter mutex\n");

		// cycle the queue
		jobcopy(temp, &jobs[0]);
		for(i = 0; i < globs->jobnum-1; i++)
		{
			jobcopy(&jobs[i], &jobs[i+1]);
		}

		if(temp->cputime != 0)
		{
			// place unfinished jobs at back of job queue
			jobcopy(&jobs[globs->jobnum-1], temp);
		}
		else	// free resource in sem EMPTY
		{
			jobcopy(&jobs[globs->jobnum-1], temp);
			globs->jobnum = globs->jobnum - 1;  // decrement job count
			v(EMPTY, jobsem);
		}

		v(MUTEX, jobsem);       // exit mutex
		//printf("dp : exit mutex\n");
		v(CPU, jobsem);		// signal CPU
		//printf("dp : signal cpu\n");
	}

	printf("dispatcher escape\n");
	v(FIN, jobsem);   // signal piece of clean up
	printf("dispatcher kill signal\n");
}

void simCPU(int ts)
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
		p(CPU, jobsem);		// wait for CPU signal
		//printf("cpu : got CPU signal\n");
		p(FULL, jobsem);	// wait for jobs in queue
		//printf("cpu : got FULL signal\n");
		p(MUTEX, jobsem);	// mutex
		//printf("cpu : enter mutex\n");

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
		jobs[0].cputime = jobs[0].cputime - timeslice;
		N = jobs[0].cputime;

		v(FULL, jobsem);        // reset full
		v(MUTEX, jobsem);	// exit mutex
		//printf("cpu : exit mutex\n");

		// confirm job completion
		if(N == 0)
		{
			printf("\t\tCPU FINISHED request for %d from %d \n", N, pid);
			v(JCALL, jobsem);  // signal job process complete
			p(JREPS, jobsem);  // wait for job reply
		}

		v(DPR, jobsem);		// signal dispatcher
		//printf("cpu : signal DPR\n");
	}

	printf("cpu escape\n");
	v(FIN, jobsem);   // signal piece of clean up
	printf("cpu kill signal\n");
}

void systemTime()
{
	// pointers to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);

	// time -> miliseconds
	clock_t start, stop;
	float elapsedTime;
	start = clock();

	*st = 0;
	float count = 1.0;
	while(globs->killswitch != 1)      // run until kill switch is activated
	{
		// update time every second
		stop = clock();
		elapsedTime = (float)(stop - start) / (float)CLOCKS_PER_SEC * 20.0;

		if(elapsedTime >= count)
		{
			*st = (int)count;	// convert to seconds
			printf("system time : %d \n", (int)count);
			count = count + 1.0;
			start = clock();
		}
	}

	// handle system clean up
	//printf("fire 0\n");
	/*
	p(FIN, jobsem);    // wait for cpu/dispatcher to stop
	printf("fire 1\n");
	p(FIN, jobsem);	
	printf("fire 2\n");
	*/

	//while(globs->subjobs != 0)
	//{/*wait for sub jobs to chill*/}

	printf("\nSystem is Shuting Down...\n\n");
	system("./cleanmess");
}

void forkHelp()
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

void setupSystem(int timeslice)
{
	// open file to hold shared memory id
	FILE *fopen(), *fp;
	
	if((fp = fopen("./share.txt","w")) == NULL)
        {
                printf("start:  could not open file.\n");
                exit(0);
        }

	// ask OS for semaphores
	jobsem = semget( IPC_PRIVATE, 8, 0777);
	
	// see if request is accepted
	if(jobsem == -1)
	{
		printf("%s", "SemGet Failed.\n");
		fclose(0);
		exit(0);
	}

	// initialize your sems
	semctl(jobsem, CPU, SETVAL, 1);		// CPU signaler
	semctl(jobsem, DPR, SETVAL, 0);         // Dispatcher signaler
        semctl(jobsem, EMPTY, SETVAL, 5);	// bounded buffer signalers
	semctl(jobsem, FULL, SETVAL, 0);
	semctl(jobsem, FIN, SETVAL, 0);		// clean up signaler
	semctl(jobsem, MUTEX, SETVAL, 1);	// mutex
	semctl(jobsem, JCALL, SETVAL, 0);
	semctl(jobsem, JREPS, SETVAL, 0);
	
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
        if(globshm == -1)
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
	a->start = b->start;
	a->end = b->end;
}
