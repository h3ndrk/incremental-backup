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

int walk(char *path, void (*callback_process_files)(char *path), void (*callback_process_directories)(char *path))
{
	struct dirent *file = NULL;
	DIR *directory = NULL;
	struct stat stats;
	char *file_path = NULL;
	
	// open directory
	if(!(directory = opendir(path)))
	{
		fprintf(stderr, "Failed to open directory: %s\n", path);
		return 1;
	}
	
	// loop through files
	while((file = readdir(directory)))
	{
		// ignore . and ..
		if(!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
		{
			continue;
		}
		
		// concatenate paths to get the absolute path
		if(!(file_path = concatenate_paths(path, file->d_name)))
		{
			continue;
		}
		
		// get entry stats
		if(lstat(file_path, &stats) < 0)
		{
			fprintf(stderr, "Failed to get stat: %s\n", file_path);
			free(file_path);
			
			continue;
		}
		
		// process entry
		if(S_ISDIR(stats.st_mode))
		{
			if(callback_process_directories != NULL)
			{
				printf("Invoke sub directory walk: %s\n", file_path);
				callback_process_directories(file_path);
				walk(file_path, callback_process_files, callback_process_directories);
			}
			
			free(file_path);
			
			continue;
		}
		else
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