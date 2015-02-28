#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <sqlite3.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"

char *concatenate_paths(char *prefix, char *suffix)
{
	char *complete_path = NULL;
	
	if(!(complete_path = malloc((strlen(prefix) + 1 + strlen(suffix) + 1) * sizeof(char))))
	{
		fprintf(stderr, "Failed to concatenate paths.\n");
		
		return NULL;
	}
	
	strcpy(complete_path, prefix);
	strcat(complete_path, "/");
	strcat(complete_path, suffix);
	
	return complete_path;
}

void remove_special_character(char *path)
{
	int i = 0;
	while(*path)
	{
		if(*path == '`')
		{
			*path = '?';
		}
		
		path++;
	}
}