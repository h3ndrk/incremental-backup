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

static struct archive *archive = NULL;

int archive_open(char *path)
{
	archive = archive_write_new();
	archive_write_add_filter_gzip(archive);
	archive_write_set_format_ustar(archive);
	archive_write_open_filename(archive, path);
	
	return 0;
}

int archive_add_file(char *path)
{
	int return_code = 0;
	struct archive_entry *entry = NULL;
	struct archive *file = NULL;
	char buff[ARCHIVE_BUFF_SIZE];
	int input_file_descriptor = 0;
	ssize_t len = 0;
	
	file = archive_read_disk_new();
	
	return_code = archive_read_disk_open(file, path);
	
	if(return_code != ARCHIVE_OK)
	{
		fprintf(stderr, "Failed to create read disk: (%s, line %i), %s\n", __FILE__, __LINE__, archive_error_string(file));
		
		return -1;
	}
	
	while(1)
	{
		entry = archive_entry_new();
		return_code = archive_read_next_header2(file, entry);
		
		if(return_code == ARCHIVE_EOF)
		{
			archive_entry_free(entry);
			break;
		}
		if(return_code != ARCHIVE_OK)
		{
			fprintf(stderr, "Failed to read file header: (%s, line %i), %s\n", __FILE__, __LINE__, archive_error_string(file));
			
			archive_entry_free(entry);
			break;
		}
		
		archive_read_disk_descend(file);
		return_code = archive_write_header(archive, entry);
		
		if(return_code < ARCHIVE_OK)
		{
			fprintf(stderr, "Failed to write archive header: (%s, line %i), %s\n", __FILE__, __LINE__, archive_error_string(file));
			
			archive_entry_free(entry);
			break;
		}
		if(return_code == ARCHIVE_FATAL)
		{
			fprintf(stderr, "Fatal error: (%s, line %i), %s\n", __FILE__, __LINE__, archive_error_string(file));
			
			archive_entry_free(entry);
			break;
		}
		if(return_code > ARCHIVE_FAILED)
		{
			input_file_descriptor = open(archive_entry_sourcepath(entry), O_RDONLY);
			len = read(input_file_descriptor, buff, ARCHIVE_BUFF_SIZE);
			while(len > 0)
			{
				archive_write_data(archive, buff, len);
				len = read(input_file_descriptor, buff, ARCHIVE_BUFF_SIZE);
			}
			close(input_file_descriptor);
		}
		
		archive_entry_free(entry);
	}
	
	archive_read_close(file);
	archive_read_free(file);
	
	return 0;
}

void archive_close(void)
{
	archive_write_close(archive);
	archive_write_free(archive);
}