//Mark Gomes
//CSC 460
//1/20/2019
//Fork 1 - C Program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
	int num = 0;

	if(argc > 1)	    //check if an argument was entered
	{
		num = atoi(argv[1]);
		if(num < 0 || num > 5)	//check if number is within range
		{
			printf("Number out of range : 0 <= N <= 5 \n");
			exit(0);
		}
	}
	else
	{
		printf("No number argument found \n");
		exit(0);
	}

	int a = 0;
	int x,y;
	printf("Gen \t PID \t PPID \n");		//set up header
	
	for(x = 1; x <= num; x++) 	//iterates throught the generations
	{
		for(y = 0; y < x; y++)		//interate through children for each generation
		{
			a = fork();		//spawn a child form parent
			if(a == 0) break;	//remove child form child spawning
		}
		if(a != 0)		//remove parents in current generation
		{
			printf("%i \t %d \t %d \n", (x-1), getpid(), getppid());
			break;
		}
	}

	if(a == 0)	//prints out info for children of current generation
		printf("%i \t %d \t %d \n", num, getpid(), getppid());
	
	sleep(1);

	return 0;
}
