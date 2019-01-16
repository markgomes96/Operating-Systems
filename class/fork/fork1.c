#include <stdio.h>
int main()
{
int a,b;

printf("Hello!!!\n");
printf("A\t\tB\t\tPID\t\tPPID\n\n");
a = fork();
b = fork();

printf("%d\t\t%d\t\t%d\t\t%d\n",a,b,getpid(),getppid());

sleep(1);

printf("Bye!!!\n");

sleep(1);
return 0;
}

