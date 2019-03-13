// Mark Gomes
// CSC 460 - Dr. Allen
// 2/28/19
// Kirby's Deadly Dinner

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

typedef enum{ false, true } bool;

typedef struct semaphore
{
	int chopsticks[5];
} semaphore;

void runSim(int shmid, int id);
void createProcesses(int *id);
int setupSystem();

int main(int argc, char** argv)
{
	int shmid, id;

	shmid = setupSystem();
	createProcesses(&id);
	runSim(shmid, id);
}

void runSim(int shmid, int id)
{
	semaphore* sem;		// get pointer to shared memory
	sem = (semaphore*) shmat(shmid, NULL, SHM_RND);	
	int right = id;		// assign left/right chopstick based on id
	int left;
	if(id == 4)
		left = 0;
	else
		left = id+1;

	int i = 0, count = 0;
	while(true)	// infinite loop
	{
		// Thinking
		for(i = 0; i < id; i++)	// tab to id column
			printf("\t");
		printf("%d : THINKING\n", id);
		while(count < 1000000)		// eat up cpu cycles
			count++;
		count = 0;		
	
		// Hungry
		for(i = 0; i < id; i++)	// tab to id column
			printf("\t");
		printf("%d : HUNGRY\n", id);

		while (true)
		{
			if(sem->chopsticks[left] == -1)
			{
				sem->chopsticks[left] = id; 	// pick up left chopstick
				break;
			}
		}

		while(count < 1000000)		// eat up cpu cycles 
			count++;
		count = 0;

		while(true)
		{
			if(sem->chopsticks[right] == -1)
			{
				sem->chopsticks[right] = id;	// pick up right chopstick
				break;
			}
		}

		// EATING
		if(sem->chopsticks[left] == id && sem->chopsticks[right] == id)
		{
			for(i = 0; i < id; i++)	// tab to id column
				printf("\t");
			printf("%d : EATING\n", id);
			while(count < 1000000)		// eat up cpu cycles
				count++;
			count = 0;

			sem->chopsticks[left] = -1;
			sem->chopsticks[right] = -1;
		}
	}
}

void createProcesses(int *id)
{
	int a = 0;
	int i;
	for(i = 1; i < 5; i++)		//create 4 children from og parent
	{
		a = fork();

		if(a == 0)		//remove child
		{
			*id = i;		//assign ids 1-4
			break;
		}
	}

	if( a != 0)
	{
		*id = 0;		// assign id 0
	}
}

int setupSystem()
{
	int shmid;
	shmid = shmget(IPC_PRIVATE, sizeof(int), 0777);	     // setup balance
	if(shmid == -1)
	{
		printf("ERROR - can't get shared memory \n");
                exit(0);
	}

	semaphore* sem;				// allocate semaphore
	sem = (semaphore*) shmat(shmid, NULL, SHM_RND);

	int i;
	for(i = 0; i < 4; i++)		// set chopstick array to -1
		sem->chopsticks[i] = -1;

	return shmid;
}
