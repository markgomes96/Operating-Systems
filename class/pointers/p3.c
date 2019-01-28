#include <stdio.h>
#include <stdlib.h>

main()
{
  int *first, *second, *combined;
  int size1, size2;
  int i;
  
/////////////////   Build a First Array //////////////////////////
  printf("How many integers in the first array? ");
  scanf("%d",&size1);

  first = (int*) malloc(size1*sizeof(int));

  for (i=0; i<size1; i++)
	first[i] = i*2;

  printf("First Array\n");
  for (i=0; i<size1; i++)
	printf("%d ",first[i]);
  printf("\n\n");

//////////////////   Build a Second Array  ////////////////////

  printf("How many integers in the second array? ");
  scanf("%d",&size2);

  second = (int*) malloc(size2*sizeof(int));

  for (i=0; i<size2; i++)
	second[i] = 100 + i;

  printf("Second Array\n");
  for (i=0; i<size2; i++)
	printf("%d ",second[i]);
  printf("\n\n");


/////////////////  Build a Combined Array  ////////////////////

  combined = (int*) malloc((size1+size2)*sizeof(int));

  for (i=0; i<size1; i++)
	combined[i] = first[i];

  for (i=0; i<size2; i++)
	combined[size1 + i] = second[i];

  printf("Combined Array\n");
  for (i=0; i<(size1+size2); i++)
	printf("%d ",combined[i]);
  printf("\n\n");
}








