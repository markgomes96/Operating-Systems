#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

#define CLASSSIZE 24  // Number of students in class (plus teacher)

struct bbStruct
{
	int id;
	char name[20];
	int favNum;
	char favFood[30];
};

main(int argc, char *argv[])
{
int shmid, i;
struct bbStruct *shmem;
FILE *fopen(), *fp;

/*****  Open File to hold Shared Memory ID  *****/

if((fp = fopen("/tmp/allen/csc460/bb/BBID.txt","w")) == NULL)
    {
    printf("start:  could not open file.\n");
    return(0);
    }

/*****  Give everyone permission to read file.  *****/
system("chmod 777 /tmp/allen/csc460/bb/BBID.txt");


/*****  Get Shared Memory and ID *****/
// 
//  Get enough shared memory so that each person in the class 
//  (plus Prof and mystery student) have
//  one 80-character line of space to write in (plus one end-of-string char)

shmid  =  shmget(IPC_PRIVATE, CLASSSIZE*sizeof(struct bbStruct), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    fclose(fp);
    return(0);
    }


/*****  Write SHMID into file and close file.  *****/

fprintf(fp,"%d\n",shmid);
fclose(fp);



/****   Attach to the shared memory  ******/

shmem = (struct bbStruct *) shmat(shmid, NULL, SHM_RND);


/***** Initialize the Shared Memory to identify each student's row.  *****/

char *who[] = {"Allen, Bob","Alvarez, Anthony", "Baszkowski, Ryan", "Bryant, Jacob", "Garcia, Alvaro", "Gomes, Mark", "Gulle, Hannah", "Hedden, Tyler", "Jellum, Chris", "Kebe, Sahid", "Law, Riley", "Le, Phuc", "Melnick, Jarrett", "Mullis, Rebecca", "Nguyen, Tyler", "Pacholczyk, Michal", "Powell, Alex", "Resha, Dylan", "Resha, Thomas", "Shankle, Preston", "Sledjeski, Maggi", "Wegner, Kyle", "Winds, Christian", "Zebell, Avery"};
for (i=0;i<CLASSSIZE;i++)
   {
printf("TOP %i\n",i);
     shmem->id = i+1;
     strcpy(shmem->name,who[i]);
     shmem->favNum = 0;
     strcpy(shmem->favFood,"");
     shmem++;
   }

/*****  PRINT MESSAGE AND TERMINATE  *****/

printf("\n\nShared Memory set up and ready for \"play.\"\n");
printf("shared-BB-ID is %d\n\n",shmid);
}










