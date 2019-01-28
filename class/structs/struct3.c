// struct1.c   example using structs

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct myVals
{
int	pid;
int	amt;
char	name[25];
};  /////// NOTE THE semicolon!

int main(int argc, char *argv[])
{
struct myVals rec1;
struct myVals rec2;
struct myVals *rec3; // NOTE: Its a pointer to a struct.
struct myVals *rec4; // NOTE: Its a pointer to a struct.

rec1.pid = getpid();
rec1.amt = 123;
strcpy(rec1.name,"Jesse");

rec2.pid = getppid();
rec2.amt = 246;
strcpy(rec2.name,"Mercer");

rec3 = &rec1;
rec4 = &rec2;

printf("\nFIRST VIEW:\n");
printf("R1(%d): %d\t%d\t%s\n",getpid(),rec1.pid,rec1.amt,rec1.name);
printf("R2(%d): %d\t%d\t%s\n",getpid(),rec2.pid,rec2.amt,rec2.name);
printf("R3(%d): %d\t%d\t%s\n",getpid(),rec3->pid,rec3->amt,rec3->name);
printf("R4(%d): %d\t%d\t%s\n",getpid(),rec4->pid,rec4->amt,rec4->name);

rec3 = (struct myVals *) malloc(sizeof(rec1));
rec3->pid = 12345;
rec3->amt = -12345;
strcpy(rec3->name,"Malloc Before Fork");

sleep(1);
printf("\nSECOND VIEW:\n");
printf("R1(%d): %d\t%d\t%s\n",getpid(),rec1.pid,rec1.amt,rec1.name);
printf("R2(%d): %d\t%d\t%s\n",getpid(),rec2.pid,rec2.amt,rec2.name);
printf("R3(%d): %d\t%d\t%s\n",getpid(),rec3->pid,rec3->amt,rec3->name);
printf("R4(%d): %d\t%d\t%s\n",getpid(),rec4->pid,rec4->amt,rec4->name);

int forkval = fork(); /////////////////// Two Procs Now ////////////////////////

rec4 = (struct myVals *) malloc(sizeof(rec1));
rec4->pid = getpid();
rec4->amt = getppid();
strcpy(rec4->name,"Malloc After Fork");


sleep(1);
printf("\nTHIRD VIEW:\n");
printf("R1(%d): %d\t%d\t%s\n",getpid(),rec1.pid,rec1.amt,rec1.name);
printf("R2(%d): %d\t%d\t%s\n",getpid(),rec2.pid,rec2.amt,rec2.name);
printf("R3(%d): %d\t%d\t%s\n",getpid(),rec3->pid,rec3->amt,rec3->name);
printf("R4(%d): %d\t%d\t%s\n",getpid(),rec4->pid,rec4->amt,rec4->name);

rec1.pid = getpid();
rec2.amt = getppid();
rec3->pid = getpid();
strcpy(rec3->name,"Both write here");
rec4->pid=getpid();
rec4->amt=forkval;

sleep(1);
printf("\nFOURTH VIEW:\n");
printf("R1(%d): %d\t%d\t%s\n",getpid(),rec1.pid,rec1.amt,rec1.name);
printf("R2(%d): %d\t%d\t%s\n",getpid(),rec2.pid,rec2.amt,rec2.name);
printf("R3(%d): %d\t%d\t%s\n",getpid(),rec3->pid,rec3->amt,rec3->name);
printf("R4(%d): %d\t%d\t%s\n",getpid(),rec4->pid,rec4->amt,rec4->name);

if (forkval == 0)
{
printf("%d is setting to zero\n",getpid());
  rec1.pid = 0;
  rec2.pid = 0;
  rec3->pid = 0;
  rec4->pid = 0;
}
else
{
  printf("%d is waiting while other sets to zero\n",getpid());
  sleep(1);
}

sleep(1);
printf("\nFIFTH VIEW:\n");
printf("R1(%d): %d\t%d\t%s\n",getpid(),rec1.pid,rec1.amt,rec1.name);
printf("R2(%d): %d\t%d\t%s\n",getpid(),rec2.pid,rec2.amt,rec2.name);
printf("R3(%d): %d\t%d\t%s\n",getpid(),rec3->pid,rec3->amt,rec3->name);
printf("R4(%d): %d\t%d\t%s\n",getpid(),rec4->pid,rec4->amt,rec4->name);

}






