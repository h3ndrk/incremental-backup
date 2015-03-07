#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <time.h> // ctime
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

static FILE *diff_file_descriptor = NULL;
static char diff_file_failed = 0; // 1 = diff file fopen failed, no more opening

void process_file(char *path)
{
	printf("%s\n", path);
}

void process_file_create_first(char *path)
{
	struct stat stats;
	
	if(diff_file_failed)
	{
		return;
	}
	
	if(diff_file_descriptor == NULL)
	{
		if((diff_file_descriptor = fopen(DIFF_FILE, "w")) == NULL)
		{
			fprintf(stderr, "Failed to open diff file.\n");
			diff_file_failed = 1;
			
			return;
		}
	}
	
	if(lstat(path, &stats) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s\n", path);
		free(path);
		
		return;
	}
	
	fprintf(diff_file_descriptor, "%li %s\n", stats.st_mtim.tv_sec, path);
}

void close_diff(void)
{
	if(diff_file_descriptor != NULL)
	{
		fclose(diff_file_descriptor);
	}
}

void process_file_final(char *path)
{
	struct stat stats;
	
	if(lstat(path, &stats) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return;
	}
	
	logic_process_file(path, stats.st_mtim.tv_sec);
}

void process_file_index(char *path)
{
	struct stat stats;
	
	if(lstat(path, &stats) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return;
	}
	
	index_add(stats.st_mtim.tv_sec, path, 0);
}