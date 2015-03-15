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
#include <utime.h> // utimes
#include <sys/time.h> // utimes

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "archive.h"
#include "index.h"
#include "arguments.h"

int main(int argc, char *argv[])
{
	time_t start_time = time(NULL);
	
	// parse arguments
	arguments_parse(argc, argv);
	
	archive_open(arguments.archive);
	
	printf("Storing files...\n");
	walk(arguments.source, process_file_check, process_directory_check);
	
	archive_close();
	
	if(arguments.show_stats)
	{
		printf("Backup statistics:\n\n    %15li files processed\n    %15li files archived\n    %15li seconds elapsed\n    %15f files per second", walked_files_amount, archived_files_amount, time(NULL) - start_time, (float)archived_files_amount / (time(NULL) - start_time));
	}
	
	// cleanup
	path_exclude_pattern_cleanup();
	arguments_cleanup();
	
	return 0;
}