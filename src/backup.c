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

void a(long long int timestamp, char *path)
{
	printf("%lli %s\n", timestamp, path);
}

int main(int argc, char *argv[])
{
	walk("/", process_file_index, NULL);
	
	if(walk_get_error())
	{
		fprintf(stderr, "Error while walking through the directory:\n    %lli open directory errors\n    %lli read file stat errors\n", walk_get_error_open_dir(), walk_get_error_read_stat());
	}
	
	index_process_file("/home/hendrik/diff.txt", process_file_index_saved);
	
	printf("%lli files in the filesystem, %lli files in index\n", index_files_get_amount(), index_saved_get_amount());
	
	// index_print();
	
	// printf("%lli\n", index_get_amount());
	
	// sleep(10);
	
	// printf("Starting...\n");
	// printf("Timestamp: %lli\n", index_get_timestamp_by_path("/home/hendrik/Programme/incremental-backup/Makefile"));
	// printf("Ended\n");
	
	index_files_cleanup();
	index_saved_cleanup();
	
	// archive_open("test.tar");
	
	// if(database_open() < 0)
	// {
	// 	return 1;
	// }
	
	// walk("/", process_file, process_file);
	
	// database_close();
	// archive_close();
	
	return 0;
}