// Mark Gomes
// CSC 460
// 2/6/2019
// Popo and Nana

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void createProcesses(int n, char** name, char** sigfl);
void waitToExecute(int cycles, int shmid, char** name, char** sigfl);

int main(int argc, char** argv)
{
	int num = 0;
	char name[10];
	char sigfl[10];
	char* np = name;
	char* sp = sigfl;

        if(argc > 1)        //check if an argument was entered
        {
                num = atoi(argv[1]);
                if(num < 1 || num > 100 )  //check if number is within range
                {
                        printf("Number out of range : 1 <= N <= 100 \n");
                        exit(0);
                }
        }
        else
        {
                printf("No number argument found \n");
                exit(0);
        }

	int shmid;
	char* ram;
        shmid = shmget(IPC_PRIVATE, 10*sizeof(char), 0777);	//setup shared memory

        if(shmid == -1)
        {
                printf("ERROR - can get shared memory \n");
                exit(0);
        }

        ram = (char*) shmat(shmid, NULL, SHM_RND);

        strcpy(ram, "Popo1");           //store starting signal

	createProcesses(num, &np, &sp);		//create 2num processes

	waitToExecute(5, shmid, &np, &sp);
}

void createProcesses(int n, char** name, char** sigfl)
{
	int a = -1;
	int x;

	for(x = 1; x < n; x++)	//create 2n processes
	{
		a = fork();
		
		if(a == 0)	//remove Popo child
		{
			snprintf(*name, sizeof *name, "Popo%i", x+1);
			snprintf(*sigfl, sizeof *sigfl, "Nana%i", x+1);			

 			break;
		}

		a = fork();

		if(a == 0)	//remove Nana child
		{
			snprintf(*name, sizeof *name, "Nana%i", x+1);
	
			if(x != n-1)
				snprintf(*sigfl, sizeof *sigfl, "Popo%i", x+2);
			else
				snprintf(*sigfl, sizeof *sigfl, "Popo1");	//loop back around

			break;
		}
	}
	if(a != 0)		//assign id to original parent
	{
		a = fork();

		if(a == 0)
		{
			snprintf(*name, sizeof *name, "Nana1");
			snprintf(*sigfl, sizeof *sigfl, "Popo2");
		}
		else
		{
			snprintf(*name, sizeof *name, "Popo1");
			snprintf(*sigfl, sizeof *sigfl, "Nana1");
		}
	}
	
}

void waitToExecute(int cycles, int shmid, char** name, char** sigfl)
{
	char *ram;
	ram = (char*) shmat(shmid, NULL, SHM_RND);

	int cycleCount = 0;
	while(cycleCount < cycles)
	{
		ram = (char*) shmat(shmid, NULL, SHM_RND);	//read shared memory

		if(strcmp(ram, *name) == 0)			//check for signal flare
		{
			printf("%s \n", *name);
			cycleCount++;		

			strcpy(ram, *sigfl);            //set off next signal flare			

			if(cycleCount == cycles && strcmp(*name, "Popo1") != 0)		//children terminate after cycles
			{
				exit(0);
			}

			if(strcmp(*sigfl, "Popo1") == 0)
                        {
                                if(cycleCount == cycles-1)              //set exit signal for final child
                                        snprintf(*sigfl, sizeof *sigfl, "exit");

                                printf("                ***%i loop \n", cycleCount);
                        }
		}
	}

	while(strcmp(ram, "exit") != 0)	   //parent node wait till all children have died
	{
		ram = (char*) shmat(shmid, NULL, SHM_RND);
	}
	printf("                ***%i loop \n", cycles);

	if ((shmctl(shmid, IPC_RMID, NULL)) == -1)	//clean up shared memory
	{
    		printf("ERROR removing shmem.\n");
		exit(0);
	}
}
