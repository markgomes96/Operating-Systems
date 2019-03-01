#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

#define CHILD 0
#define PARENT 1

int main(int argc, char *argv[])
{
if (argc == 1)
{
	printf("NO");
	return (0);
}
int N = atoi(argv[1]);

int     i, sem_id, HowManySems = N;
int     loopCount = 20000;

//  Ask OS for semaphores.
sem_id = semget (IPC_PRIVATE, HowManySems, 0777);

//  See if you got the request.
if (sem_id == -1)
   {
    printf("%s","SemGet Failed.\n");
    return (-1);
   }

//  Initialize your sems.
semctl(sem_id, 0, SETVAL, 1);
for (i=1; i<N; i++)
	semctl(sem_id, i, SETVAL, 0);

// Creat N-1 more processes with unique procNUms.
int procNum =0;
for (i=1; i<N; i++)
{
 	if (fork() != 0) break;
	procNum++;
}

int nextProc = (procNum + 1)%N;

	for (i=0; i<loopCount; i++)
	{
         p(procNum,sem_id);
         printf("%c\n",'A'+procNum);
         v(nextProc,sem_id);
	}

    // Parent needs to clean up semaphores.
    sleep(2);
    if ((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
       printf("%s", "Parent: ERROR in removing sem\n");

}

p(int s,int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = -1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = 1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}


