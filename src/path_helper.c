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

static char **path_exclude_patterns = NULL;
static long long int path_exclude_patterns_amount = 0;
static char path_skip_hidden_files_state = 0; // 1 = hidden files will be skipped

/**
 * Concatenates a prefix and a suffix path together and adds a '/' in the middle
 * @param prefix the prefix path
 * @param suffix the suffix path
 * @return an allocated concatenated path (should be freed)
 */
char *concatenate_paths(char *prefix, char *suffix)
{
	char *complete_path = NULL;
	char *prefix_copy = strdup(prefix);
	
	// remove last /
	if(prefix_copy[strlen(prefix_copy) - 1] == '/')
	{
		prefix_copy[strlen(prefix_copy) - 1] =  '\0';
	}
	
	if(!(complete_path = malloc((strlen(prefix_copy) + 1 + strlen(suffix) + 1) * sizeof(char))))
	{
		fprintf(stderr, "Failed to concatenate paths: %s, %i\n", __FILE__, __LINE__);
		
		return NULL;
	}
	
	strcpy(complete_path, prefix_copy);
	strcat(complete_path, "/");
	strcat(complete_path, suffix);
	
	free(prefix_copy);
	
	return complete_path;
}

/**
 * Fast path compare: Compares from the back which should be faster than strcmp for long paths
 * @param path0 the first path
 * @param path0_length the length of the first path
 * @param path1 the second path
 * @param path1_length the length of the second path
 * @return 1 for equality, 0 for different paths
 */
int compare_paths(char *path0, long long int path0_length, char *path1, long long int path1_length)
{
	long long int offset = 0;
	
	if(path0_length != path1_length)
	{
		return 0;
	}
	
	offset = path0_length;
	
	while(offset--)
	{
		if(*(path0 + offset) != *(path1 + offset))
		{
			return 0;
		}
	}
	
	return 1;
}

/**
 * Adds a pattern to the exclude pattern list
 * @param pattern the shell wildcard pattern
 * @return -1 on error, 0 on success
 */
int path_exclude_pattern_add(char *pattern)
{
	char **backup_path_exclude_patterns = path_exclude_patterns;
	
	printf("Excluding %s\n", pattern);
	
	if(path_exclude_patterns == NULL)
	{
		if((path_exclude_patterns = malloc((++path_exclude_patterns_amount) * sizeof(char *))) == NULL)
		{
			fprintf(stderr, "Failed to allocate exclude patterns: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	else
	{
		if((path_exclude_patterns = realloc(path_exclude_patterns, (++path_exclude_patterns_amount) * sizeof(char *))) == NULL)
		{
			path_exclude_patterns = backup_path_exclude_patterns; // restore old index
			fprintf(stderr, "Failed to reallocate exclude patterns: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	
	path_exclude_patterns[path_exclude_patterns_amount - 1] = strdup(pattern);
	
	return 0;
}

/**
 * Cleans all allocated memory from pattern exclusion
 */
void path_exclude_pattern_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < path_exclude_patterns_amount; i++)
	{
		free(path_exclude_patterns[i]);
	}
	
	free(path_exclude_patterns);
}

/**
 * Compares give path with all added exclude patterns
 * @param path the path to be tested
 * @return 1 on match, 0 when no exclude pattern matches
 */
int path_exclude_pattern_match(char *path)
{
	long long int i = 0;
	
	for(i = 0; i < path_exclude_patterns_amount; i++)
	{
		if(!fnmatch(path_exclude_patterns[i], path, 0))
		{
			return 1;
		}
	}
	
	return 0;
}

/**
 * Sets the state to skip hidden files
 * @param state 0 = disable, 1 = enable
 */
void path_skip_hidden_files(char state)
{
	path_skip_hidden_files_state = state;
}

/**
 * Returns the state to skip hidden files
 * @return 0 = disabled, 1 = enabled
 */
int path_get_skip_hidden_files(void)
{
	return path_skip_hidden_files_state;
}