#include <stdio.h>
int main()
{
int a,i;

printf("Hello!!!\n");
printf("A\tB\tPID\tPPID\n\n");
a = fork();

if (a == 0)
{
for (i = 0; i<10000; i++)
printf("%d BEAR: %d\t%d\t%d\n",i,a,getpid(),getppid());
}
else
{
for (i = 0; i<20000; i++)
printf("%d Mercer: %d\t%d\t%d\n",i,a,getpid(),getppid());
}
sleep(1);

printf("Bye!!!\n");

sleep(1);
return 0;
}

