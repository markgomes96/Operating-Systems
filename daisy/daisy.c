// Mark Gomes
// Daisy's Delightful Dining Philosphers
// CSC 460 - Dr. Allen
// 03/19/19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>

typedef enum{ false, true } bool;

enum Phil {THINKING, HUNGRY, EATING, DONE};
#define LEFT (id + 1) % 5
#define RIGHT id
#define MUTEX 5

typedef struct dinner
{
	int phils[6];	// philosophers
} dinner;

void runSim(int sem_id, int shm_id, int id);
void createProcesses(int *id);
void setupSystem(int *sem_id, int *shm_id);

p(int s, int sem_id)	// Wait
{
	struct sembuf sops;

	sops.sem_num = s;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1)
		printf("%s", "'P' error\n");
}

v(int s, int sem_id)	// Signal
{
	struct sembuf sops;
	
	sops.sem_num = s;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1)
		printf("%s", "'V' error\n");
}

void printStatus(int status)
{
	switch(status)
	{
		case 0 :
		printf("Thinking    "); break;
		case 1 :
		printf("Hungry      "); break;
		case 2 :
		printf("Eating      "); break;
		case 3 : 
		printf("Done        "); break;
	}
}

int main(int argc, char** argv)
{
	srand(time(NULL)); 	// create random seed based on time
	int sem_id, shm_id, id;

        setupSystem(&sem_id, &shm_id);
        createProcesses(&id);
        runSim(sem_id, shm_id, id);
}

void runSim(int sem_id, int shm_id, int id)
{
        dinner* din;         // get pointer to shared memory
        din = (dinner*) shmat(shm_id, NULL, SHM_RND);
        int i = 0;

	if(id == 5)	// clock fork 
	{
		int start = clock() * 1000 / CLOCKS_PER_SEC;
        	int curr = start;
        	int end = start + 81000;
        	int count = 1;

		while(curr < end)
		{
			if((curr - start) >= (count * 800))   // print every second (or so)
			{
				printf("%d. ", count);
                        	for(i = 0; i < 5; i++)  // print status of all philsophers
                        	{
                                	printStatus(din->phils[i]);
                       		}
                        	printf("\n");
                        	count++;
			}

			curr = clock() * 1000 / CLOCKS_PER_SEC;  // update time counter        
		}
		din->phils[5] = 1;	// bool to denote timer finished
	}
	else
	{
        	while(din->phils[5] == 0)     // loop for 100 seconds
        	{
			// Thinking
			din->phils[id] = THINKING;
       			sleep( ( rand() % 11 ) + 5 );	  // think 5-15 seconds

			// Hungry
			din->phils[id] = HUNGRY;

			p(MUTEX, sem_id);		// mutex
			// *** Critical Zone ***
			p(LEFT, sem_id);		// wait for left and right chopsticks
			p(RIGHT, sem_id);
			// ***      End      ***
			v(MUTEX, sem_id);		// free mutex		

			// Eating
			din->phils[id] = EATING;
			sleep( ( rand() % 3 ) + 1 );      // eat 1-3 seconds

			v(RIGHT, sem_id);		// signal left and right chopsticks
			v(LEFT, sem_id);
		}
	}
	// Done
	din->phils[id] = DONE;

	bool cleanup = true;
	for(i = 0; i < 6; i++)	// check if all done
	{
		if(din->phils[i] != DONE)
			cleanup = false;
	}
	if(cleanup)
	{
		// clean up semaphores
		if((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
			printf("%s", "Error in removing sem\n");

		// clean up shared memory
		if((shmctl(shm_id, IPC_RMID, NULL)) == -1)
			printf("Error in removing shmem");

		printf("Shared memory and semaphores cleaned \n");
	}

	if(id == 0)
		sleep(5);
}

void createProcesses(int *id)
{
        int a = 0;
        int i;
        for(i = 1; i < 6; i++)          //create 5 children from og parent
        {
                a = fork();

                if(a == 0)              //remove child
                {
                        *id = i;                //assign ids 1-5
                        break;
                }
        }

        if( a != 0)
        {
                *id = 0;                // assign id 0
        }
}

void setupSystem(int *sem_id, int *shm_id)
{
	// ask OS for semaphores
	*sem_id = semget( IPC_PRIVATE, 6, 0777);
	
	// see if you get the request
	if(*sem_id == -1)
	{
		printf("%s", "SemGet Failed.\n");
		exit(0);
	}

	// Initialize your sems
	semctl(*sem_id, 0, SETVAL, 1);	// chopstick resources
	semctl(*sem_id, 1, SETVAL, 1);
	semctl(*sem_id, 2, SETVAL, 1);
        semctl(*sem_id, 3, SETVAL, 1);
	semctl(*sem_id, 4, SETVAL, 1);
	semctl(*sem_id, MUTEX, SETVAL, 1);	// mutex

        // set up shared memory for chopstick array
        *shm_id = shmget(IPC_PRIVATE, sizeof(int), 0777);      // setup balance
        if(*shm_id == -1)
        {
                printf("ERROR - can't get shared memory \n");
                exit(0);
        }

        dinner* din;                // allocate chopstick array
        din = (dinner*) shmat(*shm_id, NULL, SHM_RND);

        int i;
	for(i = 0; i < 5; i++)
		din->phils[i] = THINKING;
	din->phils[5] = 0;
}
