// Mark Gomes
// CSC 460 - Dr. Allen
// 2/23/19
// Wario's Bank

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
	int lock;
	int balance;
} semaphore;

void runSim(int value, int num, int shmid);
void createProcesses(int *id, int *value);
void displayBalance();
void setupSystem();
void terminateSystem();
int systemCheck();

// *** Utility Functions ***

bool isNumeric(char* str)
{
	int i;
	int len = strlen(str);
	for(i = 0; i < len; i++)
	{
		if(isdigit(*str) == 0)
			return false;
		str++;
	}
	return true;
}

int power(int a, int b)
{
	if(b == 0)
		return 1;

	int value = a;
	int i;
	for(i = 1; i < b; i++)
	{
		value = value * a;
	}
	return value;
}

int main(int argc, char** argv)
{
	int num, id, value;

	if(argc > 1)	//check if arguments are entered
	{
		if(strcmp(argv[1], "balance") == 0)
		{
			displayBalance();
		}
		else if(strcmp(argv[1], "die") == 0)
		{
			terminateSystem();
		}
		else if(isNumeric(argv[1]))	//check arg for numeric
		{
			num = atoi(argv[1]);
			if(num >= 1 && num <= 100)
			{
				int shmid = systemCheck();
				createProcesses(&id,  &value);
				runSim(value, num, shmid);
			}
			else
			{
				printf("ERROR : Num outside valid range of ( 1 - 100 ) \n");
			}
				
		}
		else					//invalid arg
		{
			printf("ERROR : Invalid argument \n");
			exit(1);
		}
	}
	else
	{
		setupSystem();
	}
}

void runSim(int value, int num, int shmid)
{
	int i;
	for(i = 0; i < num; i++)	// run sim n times
	{
		// Deposit / Withdraw from balance
		int oldBal, newBal;
		semaphore* sem;
	
		while(true)
		{
        		sem = (semaphore*) shmat(shmid, NULL, SHM_RND);
			
			if(sem->lock == 0)		//balance is ready to access
				sem->lock = value;

			if(sem->lock == value)		//check if key matches lock
			{
				oldBal = sem->balance;		//modify balance
				newBal = oldBal + value;
        			sem->balance = newBal;
				sem->lock = 0;
				break;
			}
		}

		if(value > 0) 		// deposit
		{
			printf("%d + %d = %d \n", oldBal, value, newBal);
		}
		else			// withdraw
		{
			printf("	%d - %d = %d \n", oldBal, (-1*value), newBal);
		}
	}
}

void createProcesses(int *id, int *value)
{
	int a = 0;
	int i;
	for(i = 1; i < 16; i++)		//create 15 children from og parent
	{
		a = fork();

		if(a == 0)		//remove child
		{
			*id = i;	//assign values 1-15
			break;
		}
	}

	if( a != 0)
	{
		*id = 0;
	}

	// Assign deposit / withdraw value 
	*value = power(2, *id);
	a = fork();
	if(a == 0)			//set child to withdraw
		*value = -1 * (*value);
}

void displayBalance()
{
        int shmid = systemCheck(false);

        // Display balance to screen
        semaphore* sem;
        sem = (semaphore*) shmat(shmid, NULL, SHM_RND);
        printf("\nCurrent Balance : %d \n \n", sem->balance);
}

void setupSystem()
{
	// Open file
	FILE *fopen(), *fp;
	int shmid = systemCheck(true);

	if(shmid == -1)
	{
		shmid = shmget(IPC_PRIVATE, sizeof(int), 0777);		//setup balance
		if(shmid == -1)
		{
			printf("ERROR - can't get shared memory \n");
                	exit(0);
		}

		if((fp = fopen("shmem.txt","w")) == NULL)
       		{
                	printf("start:  could not open file.\n");
                	exit(0);
       		}
		system("chmod 777 shmem.txt");

		fprintf(fp, "%d\n", shmid);	//store shmid in file
		fclose(fp);

		semaphore* sem;			//set balance to 1000
		sem = (semaphore*) shmat(shmid, NULL, SHM_RND);
		sem->balance = 1000;

		printf("System has been successfully setup... Enjoy\n");
	}
	else
	{
		printf("System is already set up. \n");
	}
}

void terminateSystem()
{
        int shmid = systemCheck(false);
	
	// Begin system shutdown
	printf("*** System Shutdown Initiated *** \n \n");
	
	// Display balance to screen
	semaphore* sem;
        sem = (semaphore*) shmat(shmid, NULL, SHM_RND);
        printf("Final Balance : %d \n", sem->balance);

	// Clear up memory
	if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
		printf("ERROR removing shmem.\n");
	
	// Delete shmid file
	system("rm shmem.txt");

	printf("\n*** System Shutdown Complete ***\n");
}

int systemCheck(bool setup)
{
	//Open file
	FILE *fopen(), *fp;
	if((fp = fopen("shmem.txt","r")) == NULL)
	{
		if(!setup)
		{
			printf("start:  could not open file.\n");
			exit(0);
		}
		else
		{
			return -1;
		}
	}

	int shmid;
	fscanf(fp, "%d", &shmid);

	// Test for system set up
	if(shmid == 0 && !setup)
	{
		printf("System not yet primed for simulation \n");
		exit(1);
	}
	
	return shmid;
}
