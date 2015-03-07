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
#include "index.h"

static index_file *index_files = NULL;
static unsigned long long int index_files_amount = 0;

int index_add(long long int timestamp, char *path, char flag)
{
	index_file *backup_index = index_files;
	
	if(index_files == NULL)
	{
		if((index_files = malloc((++index_files_amount) * sizeof(index_file))) == NULL)
		{
			fprintf(stderr, "Failed to allocate index.\n");
			
			return -1;
		}
	}
	else
	{
		if((index_files = realloc(index_files, (++index_files_amount) * sizeof(index_file))) == NULL)
		{
			index_files = backup_index; // restore old index
			fprintf(stderr, "Failed to reallocate index.\n");
			
			return -1;
		}
	}
	
	index_files[index_files_amount - 1].timestamp = timestamp;
	index_files[index_files_amount - 1].path = strdup(path);
	index_files[index_files_amount - 1].flag = flag;
	
	return 0;
}

void index_cleanup(void)
{
	unsigned long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		free(index_files[i].path);
	}
	
	free(index_files);
}

void index_print(void)
{
	unsigned long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		printf("%lli %s %i\n", index_files[i].timestamp, index_files[i].path, index_files[i].flag);
	}
}