#include <stdio.h>
int main()
{
int a;

printf("Hello!!!\n");
printf("A\tPID\tPPID\n\n");
a = fork();

printf("%d\t%d\t%d\n",a,getpid(),getppid());

sleep(1);

printf("Bye!!!\n");

sleep(1);
return 0;
}

