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
static long long int index_files_amount = 0;

static int index_get_by_path(char *path);

int index_add(long long int timestamp, char *path, char flag)
{
	index_file *backup_index = index_files;
	
	if(index_files == NULL)
	{
		if((index_files = malloc((++index_files_amount) * sizeof(index_file))) == NULL)
		{
			fprintf(stderr, "Failed to allocate index: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	else
	{
		if((index_files = realloc(index_files, (++index_files_amount) * sizeof(index_file))) == NULL)
		{
			index_files = backup_index; // restore old index
			fprintf(stderr, "Failed to reallocate index: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	
	index_files[index_files_amount - 1].timestamp = timestamp;
	index_files[index_files_amount - 1].path = strdup(path);
	index_files[index_files_amount - 1].flag = flag;
	
	if(!(index_files_amount % 10000))
	{
		printf("%lli files indexed\n", index_files_amount);
	}
	
	return 0;
}

void index_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		free(index_files[i].path);
	}
	
	free(index_files);
}

void index_print(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		printf("%lli %s %i\n", index_files[i].timestamp, index_files[i].path, index_files[i].flag);
	}
}

static int index_get_by_path(char *path)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		if(!strcmp(index_files[i].path, path))
		{
			return i;
		}
	}
	
	return -1;
}

long long int index_get_timestamp_by_path(char *path)
{
	long long int index = 0;
	
	index = index_get_by_path(path);
	
	if(index < 0)
	{
		fprintf(stderr, "Failed to find file: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	if(index >= index_files_amount)
	{
		fprintf(stderr, "Failed to find file, index is out of bounds: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	return index_files[index].timestamp;
}

long long int index_get_amount(void)
{
	return index_files_amount;
}

int index_process_file(char *path, void (*callback_index_process_file)(long long int timestamp, char *path))
{
	FILE *index_file = NULL;
	char *line = NULL;
	char *line_backup = NULL;
	size_t len = 0;
	ssize_t read = 0;
	long long int read_timestamp = 0;
	long int space_position = 0;
	
	if(callback_index_process_file == NULL)
	{
		return 0;
	}
	
	if((index_file = fopen(path, "r")) == NULL)
	{
		fprintf(stderr, "Failed to open index file: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return -1;
	}
	
	while((read = getline(&line, &len, index_file)) != -1)
	{
		space_position = strchr(line, ' ') - line;
		line[space_position] = '\0';
		line_backup = line + space_position + 1;
		line_backup[strlen(line_backup) - 1] = '\0';
		read_timestamp = strtoll(line, NULL, 10);
		
		callback_index_process_file(read_timestamp, line_backup);
	}
	
	if(line)
	{
		free(line);
	}
	
	fclose(index_file);
	
	return 0;
}