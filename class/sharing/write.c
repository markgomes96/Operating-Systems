#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

main(int argc, char *argv[])
{
int  shmid = atoi(argv[1]);
char *ram;

ram = (char *) shmat(shmid,NULL, SHM_RND);

strcpy(ram, argv[2]); 

}

