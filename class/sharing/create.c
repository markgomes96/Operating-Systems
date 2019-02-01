#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

main(int argc, char *argv[])
{
int  shmid;
char *ram;

/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, 81*sizeof(char), 0777);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }
ram = (char *) shmat(shmid,NULL, SHM_RND);

strcpy(ram,"CSC 460 RAM location!!");

printf("The SHMID is: %d\nalldone\n", shmid);
}

