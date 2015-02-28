#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE 1000000
#define STRING "Testtext"
#define ITERATION_AMOUNT 1000

int main(void)
{
	char **array = NULL;
	unsigned long i = 0;
	unsigned long j = 0;
	
	array = malloc(ARRAY_SIZE * sizeof(char *));
	
	// building array
	if(!array)
	{
		fprintf(stderr, "malloc fail (root).\n");
		
		return 1;
	}
	
	for(i = 0; i < ARRAY_SIZE; i++)
	{
		array[i] = malloc((strlen(STRING) + 1) * sizeof(char));
		
		if(!array[i])
		{
			fprintf(stderr, "malloc fail.\n");
			
			return 1;
		}
		
		strcpy(array[i], STRING);
	}
	
	for(j = 0; j < ITERATION_AMOUNT; j++)
	{
		for(i = 0; i < ARRAY_SIZE; i++)
		{
			if(!strcmp(array[i], "Test"))
			{
				sleep(1);
			}
		}
	}
	
	// freeing array
	for(i = 0; i < ARRAY_SIZE; i++)
	{
		free(array[i]);
	}
	
	free(array);
	
	return 0;
}