#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <sqlite3.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"
#include "logic.h"
#include "archive.h"
#include "index.h"

int logic_process_file(char *path, long long int timestamp)
{
	if(database_file_unflag(path) < 0)
	{
		return -1;
	}
	
	if(database_file_exists(path))
	{
		switch(database_file_update(path, timestamp))
		{
			case 1: // file in index is older
			{
				printf("%s store (update)\n", path);
				archive_add_file(path);
				break;
			}
			case 0: // file in index is up to date
			{
				// printf(" is up to date\n");
				break;
			}
			case -1:
			{
				// printf(" error\n");
				
				return -1;
				break;
			}
		}
	}
	else
	{
		if(database_file_insert(path, timestamp) < 0)
		{
			// printf(" error\n");
			return -1;
		}
		
		printf("%s store (insert)\n", path);
		archive_add_file(path);
	}
	
	return 0;
}