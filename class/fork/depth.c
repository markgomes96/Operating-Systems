#include <stdio.h>
int main(int argc, char ** argv)
{
int a,i;
int N = atoi(argv[1]);

printf("Hello!!! you entered the value: %d \n",N);
printf("A\tPID\tPPID\n\n");

for (i=0; i<N; i++)
{
    a = fork();
    if (a>0) break;
}

printf("%d\t%d\t%d\n",a,getpid(),getppid());

sleep(1);

printf("Bye!!!\n");

sleep(1);
return 0;
}

