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

	if(argc > 1)
	{
		num = atoi(argv[1]);
		if(num < 0 || num > 5)
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
}
