#include <stdio.h>
#include <stdlib.h>

main()
{
int *nums;
nums = (int*) malloc(10*sizeof(int));

if (nums != NULL)
{
int i;
for (i=0;i<10;i++)
   nums[i] = i*10;

printf("Printing as an Array of ints.\n");
for (i=0;i<10;i++)
    printf("%d ",nums[i]);
printf("\n\n");

int *vals;
printf("Printing as a pointer to an int.\n");
for (vals = nums; vals < &(nums[10]) ; vals++)
    printf("%d ",*vals);
printf("\n\n");

}
}








