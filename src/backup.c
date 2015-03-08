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
#include <fnmatch.h>
#include <argp.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"
#include "logic.h"
#include "archive.h"
#include "index.h"
#include "arguments.h"

int main(int argc, char *argv[])
{
	arguments_parse(argc, argv);
	
	printf("PATH: %s\n", arguments.source);
	
	path_exclude_pattern_cleanup();
	arguments_cleanup();
	
	exit(0);
	
	printf("Storing files...\n");
	
	walk("/", process_file_index, NULL);
	
	if(walk_get_error())
	{
		fprintf(stderr, "Error while walking through the directory:\n    %lli open directory errors\n    %lli read file stat errors\n", walk_get_error_open_dir(), walk_get_error_read_stat());
	}
	
	// index_process_file("diff.txt", process_file_index_saved);
	
	printf("%lli files in the filesystem, %lli files in index\n", index_files_get_amount(), index_saved_get_amount());
	
	index_compare_files_with_index();
	
	index_write_saved("diff_out.txt");
	
	index_files_cleanup();
	index_saved_cleanup();
	path_exclude_pattern_cleanup();
	
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