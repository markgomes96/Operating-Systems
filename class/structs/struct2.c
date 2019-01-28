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

struct myVals vals[10];
int i;
int ids = 12345;

printf("\nBUILD THE ARRAY OF STRUCTS:\n");
for (i=0; i<10; i++)
{
	vals[i].pid = ids++;
	vals[i].amt = ids/(i+1);
	strcpy(vals[i].name,"Be The Bear");
	int j;
	for (j=0; j<i;j++)
		vals[i].name[11+j]='!';
	vals[i].name[11+j] = 0;  // end of string char

printf("vals[%d]: %d\t%d\t%s\n",i, vals[i].pid, vals[i].amt, vals[i].name);
}

rec1.pid = getpid();
rec1.amt = 123;
strcpy(rec1.name,"Jesse");

rec2.pid = getppid();
rec2.amt = 246;
strcpy(rec2.name,"Mercer");

rec3 = vals;

printf("\nFIRST VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

rec3 +=5;

printf("\nSECOND VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

rec3->pid = 1000;
rec3->amt = 460;
strcpy(rec3->name,"Operating Systems");


printf("\nTHIRD VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

rec3++;

printf("\nFOURTH VIEW:\n");
printf("R1: %d\t%d\t%s\n",rec1.pid,rec1.amt,rec1.name);
printf("R2: %d\t%d\t%s\n",rec2.pid,rec2.amt,rec2.name);
printf("R3: %d\t%d\t%s\n",rec3->pid,rec3->amt,rec3->name);

printf("\nVals array now...\n");
for (i=0; i<10; i++)
{
   printf("vals[%d]: %d\t%d\t%s\n",i, vals[i].pid, vals[i].amt, vals[i].name);
}


}






