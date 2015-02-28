#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp
#include <dirent.h> // opendir, readdir, closedir
#include <sys/types.h> // lstat, struct stat
#include <sys/stat.h> // lstat, struct stat
#include <unistd.h> // lstat, struct stat
#include <sqlite3.h>
#include <errno.h>

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "database.h"

static sqlite3 *database = NULL;

int database_open(void)
{
	if(sqlite3_open("database.sqlite", &database))
	{
		perror("Failed to open SQL database");
		
		return 1;
	}
	
	return 0;
}

void database_close(void)
{
	if(database != NULL)
	{
		sqlite3_close(database);
	}
}

int database_testfor_file(char *path, long long int timestamp)
{
	int exec_code = 0;
	char command[] = "SELECT * FROM files WHERE path = ? AND last_modified = ?";
	sqlite3_stmt *statement = NULL;
	
	if(sqlite3_prepare(database, command, -1, &statement, NULL) != SQLITE_OK)
	{
		perror("Failed to prepare SQL statement");
		
		return 1;
	}
	
	if(sqlite3_bind_text(statement, 1, path, -1, SQLITE_STATIC) != SQLITE_OK)
	{
		perror("Failed to bind path to SQL statement");
		
		return 1;
	}
	
	if(sqlite3_bind_int64(statement, 2, timestamp) != SQLITE_OK)
	{
		perror("Failed to bind timestamp to SQL statement");
		
		return 1;
	}
	
	exec_code = sqlite3_step(statement);
	
	if(exec_code == SQLITE_DONE)
	{
		return 1;
	}
	else if(exec_code != SQLITE_ROW)
	{
		perror("Failed to execute SQL statement");
		
		return 1;
	}
	
	if(sqlite3_finalize(statement) != SQLITE_OK)
	{
		perror("Failed to finalize SQL statement");
		
		return 1;
	}
	
	return 0;
}