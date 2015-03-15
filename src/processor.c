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

/**
 * Callback: Stores the given file into memory. Also retrieves timestamp of the file.
 * @param path the file which should be added
 */
void process_file_index(char *path)
{
	struct stat stats;
	
	// get file stats
	if(lstat(path, &stats) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return;
	}
	
	index_files_add(stats.st_mtim.tv_sec, path, 0);
}

/**
 * Callback: Stores the given file into memory
 * @param timestamp the timestamp of the file
 * @param path the file which should be added
 */
void process_file_index_saved(long int timestamp, char *path)
{
	index_saved_add(timestamp, path, 0);
}

/**
 * Callback: Checks if the file exists in index. If not it will be created. If the
 * file exists the last modified date will be checked and updated if the file in
 * the filesystem is newer.
 * @param path the path of the file that should be checked
 */
void process_file_check(char *path)
{
	struct stat stats_fs;
	struct stat stats_index;
	FILE *index_file = NULL;
	struct timeval times[2];
	char ignore = 0; // true = 1
	
	if(arguments.full)
	{
		archive_add_file(path);
		
		return;
	}
	
	char *index_path = concatenate_paths(arguments.index, path);
	
	// get filesystem stats
	if(lstat(path, &stats_fs) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		free(index_path);
		
		return;
	}
	
	// store modified time
	times[0].tv_sec = stats_fs.st_mtim.tv_sec;
	times[0].tv_usec = stats_fs.st_mtim.tv_nsec / 1000;
	times[1].tv_sec = stats_fs.st_mtim.tv_sec;
	times[1].tv_usec = stats_fs.st_mtim.tv_nsec / 1000;
	
	if(access(index_path, F_OK) == -1)
	{
		// create new file in index
		if((index_file = fopen(index_path, "w")))
		{
			fclose(index_file);
			
			utimes(index_path, times);
			
			archive_add_file(path);
		}
	}
	
	// get index stats
	if(lstat(index_path, &stats_index) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		free(index_path);
		
		return;
	}
	
	if(!ignore)
	{
		// int r = path_compare_timestamps(stats_fs.st_mtim.tv_sec, stats_fs.st_mtim.tv_nsec, stats_index.st_mtim.tv_sec, stats_index.st_mtim.tv_nsec);
		// printf("%s: { %li, %li } %s%s { %li, %li } %s\n", path, stats_fs.st_mtim.tv_sec, stats_fs.st_mtim.tv_nsec, ((r < 0)?("OLDER "):("")), ((r==0)?("="):("<>")), stats_index.st_mtim.tv_sec, stats_index.st_mtim.tv_nsec, ((r > 0)?("OLDER"):("")));
		if(path_compare_timestamps(stats_fs.st_mtim.tv_sec, stats_index.st_mtim.tv_sec) > 0)
		{
			utimes(index_path, times);
			
			archive_add_file(path);
		}
	}
	
	free(index_path);
}

void process_directory_check(char *path)
{
	char *index_path_temp = concatenate_paths(arguments.index, path);
	char *index_path = concatenate_paths(index_path_temp, "");
	free(index_path_temp);
	
	path_mkpath(index_path, 0775);
	
	free(index_path);
}