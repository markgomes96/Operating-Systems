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
	int start;
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

void p(int s, int sem_id);	// Wait
void v(int s, int sem_id);	// Signal
void jobcopy(job *a, job *b)	// copies b into a

int main(int argc, char** argv)
{
	// check arguments
	if(argc < 2)
	{
		printf("Error : Not enough arguments\n\n");
                exit(0);
	}
	int jobnum = stoi(argv[1]);
	if(jobnum < 0 || jobnum > 50)
	{
		printf("Error : Range of jobs is not within 0 - 50\n\n");
		exit(0);
	}
	
	// read in shm addresses from file
	int jobsem, timeshm, jobshm, globshm;
	
	// open file to read from
	FILE *fopen(), *fp;
	if((fp = fopen("./share.txt", "r")) == NULL)
        {
                printf("Error : could not open file.\n");
                exit(0);
        }

	// read in lines
	char* line;
	getline(&line, fp);
	jobsem = stoi(line);
	getline(&line, fp);
        timeshm = stoi(line);
	getline(&line, fp);
        jobshm = stoi(line);
	getline(&line, fp);
        globshm = stoi(line);
	fclose(fp);

	// attach to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	job* jobs = (job*) shmat(jobshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);
	
	job *temp;
	int totaltt = 0;

	if(jobnum == 0)	   // check for the kill command
	{
		globs->killswitch = 1;
		// clean up everything
		p(FIN, jobsem);  // wait for system to shutdown
		p(FIN, jobsem);
		p(FIN, jobsem);
		system("./cleanmess");
		printf("System is Shutting Down...\n\n");
	}
	else
	{
		int i, j, think, pid, cputime, start, jobind;
		int compid, comstart, comend, tt = 0;

		for(i = 0; i < N; i++)
		{
			// think for rand(2-10)
			think = rand() % 9 + 2;
			printf("\tPID is thinking %d seconds\n", think);
			sleep(think);
	
			// submit generated job
			pid = getpid() + i;
			cputime = rand() % 5 + 1;  // rand cputime 1-5
			start = *st;
			*temp = job(pid, cputime, start, -1);
			
			p(EMPTY, jobsem);	// check if queue spot is availiable
			p(MUTEX, jobsem);	// mutex
		
			// put job in next avaiable queue spot
			jobind = glob->jobnum;
			// check if job is completed
			if(jobs[jobind] != -1)
			{
				// record completed job stats
				compid = jobs[jobind].pid;
				comstart = jobs[jobind].start;
				comend = jobs[jobind].end;

				// print out turnaround time
				tt = comend - comstart;
				printf("\tPID %d FINISHED in %d", compid, tt);
			}
			jobcopy(&jobs[jobind], temp);
			glob->jobnum = jobind + 1;	// increment job count
			
			v(MUTEX, jobsem);	// exit mutex
			v(FULL, jobsem);	// signal jobs in queue
		}

		float avett = (float)totaltt / (float)jobnum;
		printf("\tPID %d is logging off with Average Turnaround Time = %2.3f \n\n",
			 getpid(), avett);
	}
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
