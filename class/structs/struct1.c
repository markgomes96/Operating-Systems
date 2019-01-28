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

rec1.pid = getpid();
rec1.amt = 123;
strcpy(rec1.name,"Jesse");

rec2.pid = getppid();
rec2.amt = 246;
strcpy(rec2.name,"Mercer");

rec3 = &rec1;

printf("FIRST VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

rec3->amt = rec1.amt + rec2.amt;

printf("\nSECOND VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name); 
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

rec3 = (struct myVals *) malloc(sizeof(struct myVals));
rec3->pid = 99999;
rec3->amt = 1000;
strcpy(rec3->name,"The Bear");

printf("\nTHIRD VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

}






