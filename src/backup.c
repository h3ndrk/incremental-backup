#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <argp.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "archive.h"
#include "index.h"
#include "arguments.h"

int main(int argc, char *argv[])
{
	char user_input = 0;
	
	arguments_parse(argc, argv);
	
	printf("Reading files from filesystem...\n");
	
	walk(arguments.source, process_file_index, NULL);
	
	if(walk_get_error())
	{
		fprintf(stderr, "Error while walking through the directory:\n    %lli open directory errors\n    %lli read file stat errors\n", walk_get_error_open_dir(), walk_get_error_read_stat());
	}
	
	if(!arguments.full)
	{
		if(index_process_file(arguments.index, process_file_index_saved) < 0)
		{
			printf("Starting full backup.\n");
		}
	}
	else
	{
		printf("Starting full backup.\n");
	}
	
	printf("%lli files in the filesystem, %lli files in index\n", index_files_get_amount(), index_saved_get_amount());
	
	printf("Continue with backup? [Y/n] ");
	if(arguments.yes)
	{
		printf("Y\n");
		user_input = '\n';
	}
	else
	{
		user_input = getchar();
	}
	
	if(user_input == '\n' || user_input == 'y' || user_input == 'Y')
	{
		archive_open(arguments.archive);
		
		index_compare_files_with_index();
		
		archive_close();
		
		index_write_saved(arguments.index);
	}
	else
	{
		printf("Interrupted by user.\n");
	}
	
	index_files_cleanup();
	index_saved_cleanup();
	path_exclude_pattern_cleanup();
	arguments_cleanup();
	
	return 0;
}