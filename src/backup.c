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
	
	if(database_files_flag())
	{
		return 1;
	}
	
	if(database_file_exists("t2"))
	{
		printf("File exists!\n");
	}
	else
	{
		printf("File does not exist!\n");
	}
	
	// if(database_file_unflag("t1"))
	// {
	// 	return 1;
	// }
	
	// if(database_file_unflag("t2"))
	// {
	// 	return 1;
	// }
	
	// if(database_file_unflag("t3"))
	// {
	// 	return 1;
	// }
	
	// if(database_files_delete_flagged())
	// {
	// 	return 1;
	// }
	
	database_close();
	
	return 0;
}