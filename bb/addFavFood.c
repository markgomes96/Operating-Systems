// Mark Gomes
// CSC 460 - Dr. Allen
// 02/18/19
// Luigi's List

#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

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
	char fFood[30];
	char* ffp = &fFood[0];
        if(argc > 1)        //check if an argument was entered
        {
                ffp = argv[1];
		if(strlen(ffp) >= 30)	//truncate string if too large
		{
			fFood[29] = '\0';
		}
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
			strcpy(shmem->favFood, ffp);
		}

		shmem++;
	}
}
