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
 */

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
	
	// parse arguments
	arguments_parse(argc, argv);
	
	printf("Reading files from filesystem...\n");
	
	// store files in filesystem into memory
	walk(arguments.source, process_file_index, NULL);
	
	if(walk_get_error())
	{
		fprintf(stderr, "Error while walking through the directory:\n    %lli open directory errors\n    %lli read file stat errors\n", walk_get_error_open_dir(), walk_get_error_read_stat());
	}
	
	if(!arguments.full)
	{
		// store file informations from index file into memory
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
		// create new archive file handle
		archive_open(arguments.archive);
		
		// compare files in memory and store them into the archive
		index_compare_files_with_index();
		
		// close archive
		archive_close();
		
		// write updated index file
		index_write_saved(arguments.index);
	}
	else
	{
		printf("Interrupted by user.\n");
	}
	
	// cleanup
	index_files_cleanup();
	index_saved_cleanup();
	path_exclude_pattern_cleanup();
	arguments_cleanup();
	
	return 0;
}