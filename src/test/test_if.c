/**
 * Copyright (C) 2015 NIPE-SYSTEMS
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

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