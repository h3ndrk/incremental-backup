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

int main(int argc, char *argv[])
{
	archive_open("test.tar");
	
	if(database_open() < 0)
	{
		return 1;
	}
	
	walk("/", process_file, process_file);
	
	database_close();
	archive_close();
	
	return 0;
}