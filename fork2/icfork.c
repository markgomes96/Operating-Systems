// Mark Gomes
// CSC 460
// 1/27/19
// Fork 2 - Bad Synch program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
	int num = 0;
	int start = 65;

        if(argc > 1)        //check if an argument was entered
        {
                num = atoi(argv[1]);
                if(num < 1 || num > 26)  //check if number is within range
                {
                        printf("Number out of range : 1 <= N <= 26 \n");
                        exit(0);
                }
        }
        else
        {
                printf("No number argument found \n");
                exit(0);
        }

	FILE *fopen(), *fp;
	
	if((fp = fopen("syncfile", "w")) == NULL)	//open file to write
	{
		printf("Couldn't open file to write. \n");
		return(0);
	}
	fprintf(fp, "%d", start);	//set 1st process to run
	fclose(fp);

        int a,x,id = 0;

	for(x = 1; x < num; x++)	//create n processes
	{
		a = fork();
		
		if(a == 0)	//remove child of current generation
		{
			id = start+x;
 			break;
		}
	}
	if(a != 0)		//assign id to original parent
		id = start;

	int cycles = 0;		//cycle through process in order 5 times
	int read = 0;
	while(cycles < 5)
	{
		if((fp = fopen("syncfile", "r")) == NULL)       //open file to read
        	{
                	printf("Couldn't open file to read. \n");
               		return 0;
        	}
		fscanf(fp, "%d", &read);		//read in file
		fclose(fp);

		if(read == id)
		{
			printf("%c : %d \n", id, getpid());	//display info

			if((fp = fopen("syncfile", "w")) == NULL)        //open file to write
        		{
                		printf("Couldn't open file to write. \n");
                		return(0);
        		}
			fprintf(fp, "%d", (read < (start+num-1)) ? (id+1) : start);		//set next process to run
			fclose(fp);

			cycles++;
		}
	}

        return 0;
}
