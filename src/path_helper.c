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

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"
#include "logic.h"
#include "archive.h"
#include "index.h"

static char **path_exclude_patterns = NULL;
static long long int path_exclude_patterns_amount = 0;
static char path_skip_hidden_files_state = 0; // 1 = hidden files will be skipped

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

int path_exclude_pattern_add(char *pattern)
{
	char **backup_path_exclude_patterns = path_exclude_patterns;
	
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

void path_exclude_pattern_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < path_exclude_patterns_amount; i++)
	{
		free(path_exclude_patterns[i]);
	}
	
	free(path_exclude_patterns);
}

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

void path_skip_hidden_files(char state) // 0 = disable, 1 = enable
{
	path_skip_hidden_files_state = state;
}

int path_get_skip_hidden_files(void)
{
	return path_skip_hidden_files_state;
}