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
#include "logic.h"

int main(int argc, char *argv[])
{
	
	if(database_open() < 0)
	{
		return 1;
	}
	
	walk("/home/hendrik/Programme/can", process_file_final, process_file);
	// if(database_files_flag() < 0)
	// {
	// 	return 1;
	// }
	
	// if(database_file_exists("t2"))
	// {
	// 	printf("File exists!\n");
	// }
	// else
	// {
	// 	printf("File does not exist!\n");
	// }
	
	// printf("Return code: %i\n", database_file_update("Testfile43", 65000));
	
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