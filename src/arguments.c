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

static error_t arguments_parse_opt(int key, char *arg, struct argp_state *state);

const char *argp_program_version = "backup 0.0.1";
const char *argp_program_bug_address = "https://github.com/NIPE-SYSTEMS/incremental-backup/issues";
static const char doc[] = "Simple incremental backup implementation in C.";
static struct argp_option options[] =
{
	{ "test", 't', 0, 0, "Testings...", 0 },
	{ 0 }
};
struct arguments
{
	int number;
};
struct arguments arguments = { 0 };

static error_t arguments_parse_opt(int key, char *arg, struct argp_state *state)
{
	// Get the input argument from argp_parse, which we know is a pointer to our arguments structure.
	struct arguments *arguments = state->input;
	
	switch(key)
	{
		case 't':
		{
			arguments->number = 1;
			break;
		}
		default:
		{
			return ARGP_ERR_UNKNOWN;
		}
	}
	
	return 0;
}

void arguments_parse(int argc, char **argv)
{
	struct argp argp = { options, arguments_parse_opt, 0, doc, 0, 0, 0 };
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
}