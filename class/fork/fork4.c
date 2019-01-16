#include <stdio.h>
int main()
{
int a,i,j,k;

printf("Hello!!!\n");
printf("A\tB\tPID\tPPID\n\n");
a = fork();

if (a == 0)
{
for (i = 0; i<50; i++)
{
printf("%d BEAR: %d\t%d\t%d\n",i,a,getpid(),getppid());
for(k=0;k<99999;k++)
    {j++; j--; j++; j--;}
}
}
else
{
for (i = 0; i<50; i++)
{
printf("%d Mercer: %d\t%d\t%d\n",i,a,getpid(),getppid());
for(k=0;k<99;k++)
    {j++; j--; j++; j--;}
}
}
sleep(3);

printf("Bye!!!\n");

sleep(1);
return 0;
}

