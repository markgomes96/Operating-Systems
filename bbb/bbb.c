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
} job;

void dispatch(int jobsem, int jobshm);
void simCPU(int jobsem, int jobshm);
void systemTime(int timeshm);
void forkHelp(int *id);
void setupSystem(int *jobsem, int *timeshm, int *jobshm);

void p(int s, int sem_id);	// Wait
void v(int s, int sem_id);	// Signal

int main(int argc, char** argv)
{
	srand(time(NULL)); 	// create random seed based on time
	int jobsem, timeshm, jobshm, id;
	
	// set up shared memory for system
        setupSystem(&jobsem, &timeshm, &jobshm);

	printf("Let the Simulation Begin!! \n\n");
	// fork off helps to preform tasks based on id
	forkHelp(&id);
	switch(id)
	{
		case 0:
		dispatch(jobsem, jobshm);
		break;

		case 1:
		simCPU(jobsem, jobshm);
		break;

		case 2:
		systemTime(timeshm);
		break;

		default:
		break;
	}
}

// kill switch is set system time to -1
void dispatch(int jobsem, int jobshm)
{
	//
}

void simCPU(int jobsem, int jobshm)
{
	//
}

void systemTime(int timeshm)
{
	// pointer to shared memory
	int* st = (int*) shmat(timeshm, NULL, SHM_RND);

	// time -> miliseconds
	clock_t start = clock() / CLOCKS_PER_SEC;
	clock_t curr = start;
	
	*st = curr;
	int count = 1;
	while(count < 10) /*(*st != -1)*/      // update time every second
	{
		if((curr - start) >= count * 1.0)
		{
			*st = count;	// convert to seconds
			printf("system time : %d \n", count);
			count++;
		}
		curr = clock() / CLOCKS_PER_SEC;
	}
}

void forkHelp(int *id)
{
	int a = 0;
        int i;
        for(i = 1; i < 3; i++)          //create 2 children from og parent
        {
                a = fork();

                if(a == 0)              //remove child
                {
                        *id = i;                //assign ids 1-2
                        break;
                }
        }

        if( a != 0)
        {
                *id = 0;                // assign id 0
        }
}

void setupSystem(int *jobsem, int *timeshm, int *jobshm)
{
	// open file to hold shared memory id
	FILE *fopen(), *fp;
	
	if((fp = fopen("./share.txt","w")) == NULL)
        {
                printf("start:  could not open file.\n");
                exit(0);
        }

	// ask OS for semaphores
	*jobsem = semget( IPC_PRIVATE, 5, 0777);
	
	// see if request is accepted
	if(*jobsem == -1)
	{
		printf("%s", "SemGet Failed.\n");
		fclose(0);
		exit(0);
	}

	// initialize your sems
	semctl(*jobsem, 0, SETVAL, 0);
	semctl(*jobsem, 1, SETVAL, 0);
	semctl(*jobsem, 2, SETVAL, 0);
        semctl(*jobsem, 3, SETVAL, 0);
	semctl(*jobsem, 4, SETVAL, 0);
	//semctl(*sem_id, MUTEX, SETVAL, 1);	// mutex
	
	// set up shared memory for job buffer
	*jobshm = shmget(IPC_PRIVATE, 5*sizeof(struct job), 0777);
	if(*jobshm == -1)
        {
                printf("ERROR - can't get shared memory \n");
		fclose(fp);
                exit(0);
        }

	// set up shared memory for system time
	*timeshm = shmget(IPC_PRIVATE, sizeof(int), 0777);
        if(*timeshm == -1)
        {
                printf("ERROR - can't get shared memory \n");
		fclose(0);
                exit(0);
        }

	fprintf(fp, "%d\n", jobsem);
	fprintf(fp, "%d\n", timeshm);
	fprintf(fp, "%d\n", jobshm);
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
