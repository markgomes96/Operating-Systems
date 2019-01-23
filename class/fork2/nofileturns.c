#include <stdio.h>

main(int argc, char *argv[])
{

/*****  Create Child process and give unique myID *****/

int  myID = 0;
int  otherID = 1;

if (fork() == 0)
{
   myID = 1;
   otherID = 0;
}


/*****  LOOP 100 Times  *****/

int  i = 0;
int  found;

for (i=0; i<100; i++)
{

     // It must be my turn to do something.....

     printf("%d: %d\n",myID,getpid());

}

    return(0);

}
