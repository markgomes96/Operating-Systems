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
	int start;
	int end;
} job;

typedef struct global
{
	int timeslice;
	int jobnum;
	int killswitch;
	int subjobs;
} global;

void p(int s, int sem_id);	// Wait
void v(int s, int sem_id);	// Signal
void jobcopy(job *a, job *b);	// copies b into a

int main(int argc, char** argv)
{
	srand(time(NULL));	// create random seed based on time
	// check arguments
	if(argc < 2)
	{
		printf("Error : Not enough arguments\n\n");
                exit(0);
	}
	int jobnum = atoi(argv[1]);
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
	char buffer[32];
	char *b = buffer;
	size_t buffsize = 32;
	getline(&b, &buffsize, fp);
	jobsem = atoi(b);
	getline(&b, &buffsize, fp);
        timeshm = atoi(b);
	getline(&b, &buffsize, fp);
        jobshm = atoi(b);
	getline(&b, &buffsize, fp);
        globshm = atoi(b);
	fclose(fp);

	// attach to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);
	job* jobs = (job*) shmat(jobshm, NULL, SHM_RND);
	global* globs = (global*) shmat(globshm, NULL, SHM_RND);
	
	int totaltt = 0;
	job j;
	job *temp = &j;

	if(jobnum == 0)	   // check for the kill command
	{
		globs->killswitch = 1;
	}
	else
	{
		//subscribe job to system
		globs->subjobs++;

		int i, j, think, pid, cputime, start, jobind;
		int compid, comstart, comend, tt = 0, timer = 0;

		for(i = 0; i < jobnum; i++)
		{
			// think for rand(2-10)
			think = rand() % 9 + 2;
			printf("\tPID is thinking %d seconds\n", think);

			timer = *st + think;  // sleep for think seconds
                	while(*st < timer)
                	{/**/}
	
			p(EMPTY, jobsem);	// check if queue spot is available
			printf("\tuser : got EMPTY signal\n");
			p(MUTEX, jobsem);	// mutex
			printf("\tuser : enter mutex\n");

			// submit generated job
			pid = getpid();
			cputime = rand() % 5 + 1;  // rand cputime 1-5
			start = *st;
			temp->pid = pid;
			temp->cputime = cputime;
			temp->start = start;
			temp->end = -1;
		
			// put job in next avaiable queue spot
			jobind = globs->jobnum;
			jobcopy(&jobs[jobind], temp);
			globs->jobnum = jobind + 1;     // increment job count
		
			printf("\tsubmited job\n");
			
			v(MUTEX, jobsem);       // exit mutex
			printf("\tuser : exit mutex\n");
                        v(FULL, jobsem);        // signal jobs in queue
			printf("\tuser : signal FULL\n");

			// wait until job is complete
			while(1)
			{
				p(JCALL, jobsem);  // wait for cpu signal
				printf("\tuser : recieved JCALL signal\n");

				//if(jobs[0].pid == pid)
				//{
					// record completed job stats
					compid = jobs[jobind].pid;
					comstart = jobs[jobind].start;
					comend = jobs[jobind].end;

					// print out turnaround time
					tt = comend - comstart;
					printf("\tPID %d FINISHED in %d", compid, tt);

					v(JREPS, jobsem);  // signal cpu to continue
					printf("\tuser : signal JREPS\n");
					//break;  // break out of loop
				//}
				//else
				//{
					printf("\tuser : pids dont match %d %d\n", jobs[0].pid, getpid());
					break;
				//}
			}
		}

		float avett = (float)totaltt / (float)jobnum;
		printf("\tPID %d is logging off with Average Turnaround Time = %2.3f \n\n",
			 getpid(), avett);
		globs->subjobs--;
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
	a->start = b->start;
	a->end = b->end;
}
