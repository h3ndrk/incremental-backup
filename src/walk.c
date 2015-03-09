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

static char error = 0;
static long long int error_open_dir = 0;
static long long int error_read_stat = 0;

/**
 * Walks recursively through a directory and calls callback functions for each file or directory
 * @param path the path of the top level directory
 * @param callback_process_files the file callback
 * @param callback_process_directories the directory callback
 * @return -1 on error, 0 on success
 */
int walk(char *path, void (*callback_process_files)(char *path), void (*callback_process_directories)(char *path))
{
	struct dirent *file = NULL;
	DIR *directory = NULL;
	struct stat stats;
	char *file_path = NULL;
	
	// resetting error
	error = 0;
	
	// open directory
	if(!(directory = opendir(path)))
	{
		error = 1;
		error_open_dir++;
		
		fprintf(stderr, "Failed to open directory: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		return -1;
	}
	
	// loop through files
	while((file = readdir(directory)))
	{
		// ignore . and ..
		if(!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
		{
			continue;
		}
		
		// skip hidden files (begins with point)
		if(path_get_skip_hidden_files() && *(file->d_name) == '.')
		{
			continue;
		}
		
		// concatenate paths to get the absolute path
		if(!(file_path = concatenate_paths(path, file->d_name)))
		{
			continue;
		}
		
		// exclude files
		if(path_exclude_pattern_match(file_path))
		{
			printf("Skipping %s\n", file_path);
			
			free(file_path);
			
			continue;
		}
		
		// get entry stats
		if(lstat(file_path, &stats) < 0)
		{
			error = 1;
			error_read_stat++;
			
			fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", file_path, __FILE__, __LINE__);
			free(file_path);
			
			continue;
		}
		
		// process file entry
		if(S_ISDIR(stats.st_mode))
		{
			if(callback_process_directories != NULL)
			{
				// printf("Invoke sub directory walk: %s, (%s, line %i)\n", file_path, __FILE__, __LINE__);
				callback_process_directories(file_path);
			}
			
			walk(file_path, callback_process_files, callback_process_directories);
			
			free(file_path);
			
			continue;
		}
		if(S_ISREG(stats.st_mode))
		{
			if(callback_process_files != NULL)
			{
				callback_process_files(file_path);
			}
		}
		
		free(file_path);
	}
	
	closedir(directory);
	
	return 0;
}

/**
 * Returns the error state of the previous directory walking
 * @return 0 for no error, 1 for occurred errors
 */
int walk_get_error(void)
{
	return error;
}

/**
 * Returns the amount of open dir errors
 * @return the amount
 */
long long int walk_get_error_open_dir(void)
{
	return error_open_dir;
}

/**
 * Returns the amount of read stat errors
 * @return the amount
 */
long long int walk_get_error_read_stat(void)
{
	return error_read_stat;
}

