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
#include <argp.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"
#include "logic.h"
#include "archive.h"
#include "index.h"
#include "arguments.h"

static index_file *index_files = NULL;
static long long int index_files_amount = 0;

static index_file *index_saved = NULL;
static long long int index_saved_amount = 0;

static long long int index_files_get_by_path(char *path, long long int path_length);
static long long int index_saved_get_by_path(char *path, long long int path_length);

int index_files_add(long long int timestamp, char *path, char flag)
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
	index_files[index_files_amount - 1].path_length = strlen(path);
	index_files[index_files_amount - 1].flag = flag;
	
	if(!(index_files_amount % 100000))
	{
		printf("%lli files indexed\n", index_files_amount);
	}
	
	return 0;
}

void index_files_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		free(index_files[i].path);
	}
	
	free(index_files);
}

static long long int index_files_get_by_path(char *path, long long int path_length)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		if(compare_paths(index_files[i].path, index_files[i].path_length, path, path_length))
		{
			return i;
		}
	}
	
	return -1;
}

long long int index_files_get_timestamp_by_path(char *path, long long int path_length)
{
	long long int index = 0;
	
	index = index_files_get_by_path(path, path_length);
	
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

long long int index_files_get_timestamp_by_index(long long int index)
{
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

long long int index_files_get_amount(void)
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

int index_saved_add(long long int timestamp, char *path, char flag)
{
	index_file *backup_index = index_saved;
	
	if(index_saved == NULL)
	{
		if((index_saved = malloc((++index_saved_amount) * sizeof(index_file))) == NULL)
		{
			fprintf(stderr, "Failed to allocate index: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	else
	{
		if((index_saved = realloc(index_saved, (++index_saved_amount) * sizeof(index_file))) == NULL)
		{
			index_saved = backup_index; // restore old index
			fprintf(stderr, "Failed to reallocate index: (%s, line %i)\n", __FILE__, __LINE__);
			
			return -1;
		}
	}
	
	index_saved[index_saved_amount - 1].timestamp = timestamp;
	index_saved[index_saved_amount - 1].path = strdup(path);
	index_saved[index_saved_amount - 1].path_length = strlen(path);
	index_saved[index_saved_amount - 1].flag = flag;
	
	if(!(index_saved_amount % 100000))
	{
		printf("%lli saved files indexed\n", index_saved_amount);
	}
	
	return 0;
}

void index_saved_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_saved_amount; i++)
	{
		free(index_saved[i].path);
	}
	
	free(index_saved);
}

static long long int index_saved_get_by_path(char *path, long long int path_length)
{
	long long int i = 0;
	
	for(i = 0; i < index_saved_amount; i++)
	{
		if(compare_paths(index_saved[i].path, index_saved[i].path_length, path, path_length))
		{
			return i;
		}
	}
	
	return -1;
}

long long int index_saved_get_timestamp_by_path(char *path, long long int path_length)
{
	long long int index = 0;
	
	index = index_saved_get_by_path(path, path_length);
	
	if(index < 0)
	{
		return -1;
	}
	
	if(index >= index_saved_amount)
	{
		fprintf(stderr, "Failed to find file, index is out of bounds: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	return index_saved[index].timestamp;
}

long long int index_saved_get_timestamp_by_index(long long int index)
{
	if(index < 0)
	{
		return -1;
	}
	
	if(index >= index_saved_amount)
	{
		fprintf(stderr, "Failed to find file, index is out of bounds: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	return index_saved[index].timestamp;
}

long long int index_saved_get_amount(void)
{
	return index_saved_amount;
}

void index_compare_files_with_index(void)
{
	long long int i = 0;
	long long int timestamp = 0;
	long long int index = 0;
	
	printf("Comparing files...\n");
	
	for(i = 0; i < index_files_amount; i++)
	{
		index = index_saved_get_by_path(index_files[i].path, index_files[i].path_length);
		timestamp = index_saved_get_timestamp_by_index(index);
		
		// flag found files
		if(index >= 0)
		{
			index_saved[index].flag = 1;
		}
		
		// add or update files in index
		if(timestamp < 0)
		{
			printf("%s\n", index_files[i].path);
			archive_add_file(index_files[i].path);
			index_saved_add(index_files[i].timestamp, index_files[i].path, 1);
			
			continue;
		}
		if(timestamp < index_files[i].timestamp)
		{
			printf("%s\n", index_files[i].path);
			archive_add_file(index_files[i].path);
			index_saved[index].timestamp = index_files[i].timestamp;
			
			continue;
		}
	}
}

int index_write_saved(char *path)
{
	long long int i = 0;
	FILE *saved_file_descriptor = NULL;
	
	printf("Writing index to %s\n", path);
	
	if((saved_file_descriptor = fopen(path, "w")) == NULL)
	{
		fprintf(stderr, "Failed to open diff file: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	for(i = 0; i < index_saved_amount; i++)
	{
		if(index_saved[i].flag == 1)
		{
			fprintf(saved_file_descriptor, "%lli %s\n", index_saved[i].timestamp, index_saved[i].path);
		}
	}
	
	fclose(saved_file_descriptor);
	
	return 0;
}