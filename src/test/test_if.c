#include <stdio.h>

void test_condition(int value)
{
	printf("Testing condition for x = %i: ", value);
	
	if(value)
	{
		printf("true");
	}
	else
	{
		printf("false");
	}
	
	printf("\n");
}

int main(void)
{
	test_condition(-10);
	test_condition(-1);
	test_condition(0);
	test_condition(1);
	test_condition(10);
	
	return 0;
}