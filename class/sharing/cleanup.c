#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/shm.h>


main(int argc, char *argv[])
{
int shmid = atoi(argv[1]);

if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");

printf("alldone\n");
}

