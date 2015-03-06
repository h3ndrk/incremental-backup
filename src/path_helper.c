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