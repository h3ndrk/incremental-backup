#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <sqlite3.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"

int main(int argc, char *argv[])
{
	// walk("/home/hendrik/Programme/can", process_file_create_first, process_file);
	// close_diff();
	
	if(database_open())
	{
		return 1;
	}
	
	printf("Return code: %i\n", database_testfor_file("testfile2", 123456789));
	
	database_close();
	
	return 0;
}