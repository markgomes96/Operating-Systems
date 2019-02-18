// Mark Gomes
// CSC 460 - Dr. Allen
// 02/18/19
// Luigi's List

#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <limits.h>

#define CLASS_SIZE 24

struct bbStruct
{
	int id;
	char name[20];
	int favNum;
	char favFood[30];
};

main(int argc, char *argv[])
{
	//Check for command argument
	long input = 0;
	int fnum = 0;
        if(argc > 1)        //check if an argument was entered
        {
                input = atoi(argv[1]);
                if(input < INT_MIN || input > INT_MAX)  //check if number is within range
                {
                        printf("Number out of int range\n");
			return(0);
                }
		fnum = (int)input;
        }
        else
        {
                printf("No number argument found \n");
                return(0);
        }

	int shmid;
	int i;
	struct bbStruct *shmem;
	FILE *fopen(), *fp;

	//Open file that holds Shared Memory ID
	if((fp = fopen("/tmp/allen/csc460/bb/BBID.txt", "r")) == NULL)
	{
		printf("watch: could not open file. \n");
		return(0);
	}

	//Get Shared Memory ID
	fscanf(fp, "%d", &shmid);
	fclose(fp);

	//Attach to the shared memory
	shmem = (struct bbStruct *) shmat(shmid, NULL, SHM_RND);

	for (i = 0; i < CLASS_SIZE; i++)
	{
		if(strstr(shmem->name, "Gomes, Mark"))
		{
			shmem->favNum = fnum;
		}

		shmem++;
	}
}
