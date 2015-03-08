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

void process_file_index(char *path)
{
	struct stat stats;
	
	if(lstat(path, &stats) < 0)
	{
		fprintf(stderr, "Failed to get stat: %s, (%s, line %i)\n", path, __FILE__, __LINE__);
		
		return;
	}
	
	index_files_add(stats.st_mtim.tv_sec, path, 0);
}

void process_file_index_saved(long long int timestamp, char *path)
{
	index_saved_add(timestamp, path, 0);
}