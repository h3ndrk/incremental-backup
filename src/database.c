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
		fprintf(stderr, "Failed to open SQL database: %s, line %i\n", __FILE__, __LINE__);
		
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

int database_file_exists(char *path)
{
	printf("Checking file: %s...\n", path);
	
	sqlite3_stmt *statement = NULL;
	int exec_code = 0;
	
	if(sqlite3_prepare(database, "SELECT * FROM files WHERE path = ?", -1, &statement, NULL) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to prepare SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	if(sqlite3_bind_text(statement, 1, path, -1, SQLITE_STATIC) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to bind path to SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		if(sqlite3_finalize(statement) != SQLITE_OK)
		{
			fprintf(stderr, "Failed to finalize SQL statement: %s, line %i\n", __FILE__, __LINE__);
			
			return 1;
		}
		
		return 1;
	}
	
	printf("Modified rows: %i\n", sqlite3_changes(database));
	exec_code = sqlite3_step(statement);
	
	if(exec_code != SQLITE_DONE && exec_code != SQLITE_ROW)
	{
		fprintf(stderr, "Failed to execute SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	printf("Modified rows: %i\n", sqlite3_changes(database));
	
	if(sqlite3_finalize(statement) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to finalize SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	return 0;
}

int database_files_flag(void)
{
	printf("Flagging files...\n");
	
	if(sqlite3_exec(database, "UPDATE files SET flag = 1", NULL, NULL, NULL) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to execute SQL update: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	return 0;
}

int database_file_unflag(char *path)
{
	printf("Unflagging file: %s...\n", path);
	
	sqlite3_stmt *statement = NULL;
	
	if(sqlite3_prepare(database, "UPDATE files SET flag = 0 WHERE path = ?", -1, &statement, NULL) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to prepare SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	if(sqlite3_bind_text(statement, 1, path, -1, SQLITE_STATIC) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to bind path to SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		if(sqlite3_finalize(statement) != SQLITE_OK)
		{
			fprintf(stderr, "Failed to finalize SQL statement: %s, line %i\n", __FILE__, __LINE__);
			
			return 1;
		}
		
		return 1;
	}
	
	if(sqlite3_step(statement) != SQLITE_DONE)
	{
		fprintf(stderr, "Failed to execute SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	if(sqlite3_finalize(statement) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to finalize SQL statement: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	return 0;
}

int database_files_delete_flagged(void)
{
	printf("Delete flagged files...\n");
	
	if(sqlite3_exec(database, "DELETE FROM files WHERE flag = 1", NULL, NULL, NULL) != SQLITE_OK)
	{
		fprintf(stderr, "Failed to execute SQL update: %s, line %i\n", __FILE__, __LINE__);
		
		return 1;
	}
	
	return 0;
}