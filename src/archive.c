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
#include <utime.h> // utimes
#include <sys/time.h> // utimes

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "archive.h"
#include "index.h"
#include "arguments.h"

static struct archive *archive = NULL;
long int archived_files_amount = 0;

/**
 * Intializes a new archive to store files
 * @param path the path to the new archive
 * @return always 0
 */
int archive_open(char *path)
{
	archive = archive_write_new();
	
	switch(arguments.compression_algorithm)
	{
		case ARGUMENTS_ALG_B64ENCODE:
		{
			archive_write_add_filter_b64encode(archive);
			break;
		}
		case ARGUMENTS_ALG_BZIP2:
		{
			archive_write_add_filter_bzip2(archive);
			break;
		}
		case ARGUMENTS_ALG_COMPRESS:
		{
			archive_write_add_filter_compress(archive);
			break;
		}
		case ARGUMENTS_ALG_GRZIP:
		{
			archive_write_add_filter_grzip(archive);
			break;
		}
		case ARGUMENTS_ALG_GZIP:
		{
			archive_write_add_filter_gzip(archive);
			break;
		}
		case ARGUMENTS_ALG_LRZIP:
		{
			archive_write_add_filter_lrzip(archive);
			break;
		}
		case ARGUMENTS_ALG_LZIP:
		{
			archive_write_add_filter_lzip(archive);
			break;
		}
		case ARGUMENTS_ALG_LZOP:
		{
			archive_write_add_filter_lzop(archive);
			break;
		}
		case ARGUMENTS_ALG_UUENCODE:
		{
			archive_write_add_filter_uuencode(archive);
			break;
		}
		case ARGUMENTS_ALG_XZ:
		{
			archive_write_add_filter_xz(archive);
			break;
		}
	}
	
	archive_write_set_format_ustar(archive);
	archive_write_open_filename(archive, path);
	
	return 0;
}

/**
 * Adds a file to the archive
 * @param path the path to the file which should be added
 * @return -1 on error, 0 on success
 */
int archive_add_file(char *path)
{
	if(arguments.verbose)
	{
		printf("%s\n", path);
	}
	
	archived_files_amount++;
	
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
			// copy file into archive
			input_file_descriptor = open(path, O_RDONLY);
			if(input_file_descriptor < 0)
			{
				fprintf(stderr, "Failed to open write stream: (%s, line %i)\n", __FILE__, __LINE__);
				
				archive_entry_free(entry);
				break;
			}
			
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

/**
 * Cleans all allocated memory from archive creation
 */
void archive_close(void)
{
	archive_write_close(archive);
	archive_write_free(archive);
}