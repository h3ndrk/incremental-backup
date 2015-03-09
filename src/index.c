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

static index_file *index_files = NULL;
static long long int index_files_amount = 0;

static index_file *index_saved = NULL;
static long long int index_saved_amount = 0;

static long long int index_files_get_by_path(char *path, long long int path_length);
static long long int index_saved_get_by_path(char *path, long long int path_length);

/**
 * Adds a file to the file list of files from filesystem
 * @param timestamp the timestamp of the file
 * @param path the path of the file
 * @param flag a flag used for internal comparing
 * @return -1 on error, 0 on success
 */
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

/**
 * Cleans all allocated memory from file indexing
 */
void index_files_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_files_amount; i++)
	{
		free(index_files[i].path);
	}
	
	free(index_files);
}

/**
 * Retrieves array-id of the first file found by path
 * @param path the path of the file to search
 * @param path_length the length of the path
 * @return the found array-id or -1 when nothing found
 */
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

/**
 * Retrieves timestamp of the first file found by path
 * @param path the path of the file to search
 * @param path_length the length of the path
 * @return the found timestamp or -1 when nothing found
 */
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

/**
 * Retrieves array-id of the first file found by array-id
 * @param index the array-id of the file
 * @return the found timestamp or -1 when nothing found
 */
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

/**
 * Returns the amount of files in the index
 * @return the amount
 */
long long int index_files_get_amount(void)
{
	return index_files_amount;
}

/**
 * Reads index file and stores all entries into memory
 * @param path the path of the index file
 * @param callback a callback function which should be executed for each entry in the index file
 * @return -1 on error, 0 on success
 */
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
	
	printf("Reading files from index...\n");
	
	// open index file
	if((index_file = fopen(path, "r")) == NULL)
	{
		fprintf(stderr, "Failed to open index file: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return -1;
	}
	
	// read every line
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

/**
 * Adds a file to the file list of files from index file
 * @param timestamp the timestamp of the file
 * @param path the path of the file
 * @param flag a flag used for internal comparing
 * @return -1 on error, 0 on success
 */
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

/**
 * Cleans all allocated memory from file indexing
 */
void index_saved_cleanup(void)
{
	long long int i = 0;
	
	for(i = 0; i < index_saved_amount; i++)
	{
		free(index_saved[i].path);
	}
	
	free(index_saved);
}

/**
 * Retrieves array-id of the first file found by path
 * @param path the path of the file to search
 * @param path_length the length of the path
 * @return the found array-id or -1 when nothing found
 */
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

/**
 * Retrieves timestamp of the first file found by path
 * @param path the path of the file to search
 * @param path_length the length of the path
 * @return the found timestamp or -1 when nothing found
 */
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

/**
 * Retrieves array-id of the first file found by array-id
 * @param index the array-id of the file
 * @return the found timestamp or -1 when nothing found
 */
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

/**
 * Returns the amount of files in the index
 * @return the amount
 */
long long int index_saved_get_amount(void)
{
	return index_saved_amount;
}

/**
 * Compares all files in memory and stores files which are missing
 */
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

/**
 * Writes a new index file to the given path
 * @param path the path to the index file
 * @return -1 on error, 0 on success
 */
int index_write_saved(char *path)
{
	long long int i = 0;
	FILE *saved_file_descriptor = NULL;
	
	printf("Writing index to %s\n", path);
	
	// open index file
	if((saved_file_descriptor = fopen(path, "w")) == NULL)
	{
		fprintf(stderr, "Failed to open diff file: (%s, line %i)\n", __FILE__, __LINE__);
		
		return -1;
	}
	
	// write every entry from memory
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