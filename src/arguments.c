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

static error_t arguments_parse_opt(int key, char *arg, struct argp_state *state);

const char *argp_program_version = "backup 0.0.1";
const char *argp_program_bug_address = "https://github.com/NIPE-SYSTEMS/incremental-backup/issues";
static const char *doc = "Simple incremental backup implementation in C.";
static struct argp_option options[] =
{
	{ NULL, 0, NULL, 0, "I/O options", 1 },
	{ "source", 's', "PATH", 0, "Path to the directory which should be stored in backup (required)", 0 },
	{ "archive", 'a', "FILE", 0, "Path to the generated archive (required)", 0 },
	{ "index", 'i', "FILE", 0, "Path to the index file (required)", 0 },
	{ NULL, 0, NULL, 0, "Recursivly directory walking options", 2 },
	{ "exclude", 'e', "PATTERN", 0, "Exclude files and directories which match the shell wildcard pattern PATTERN", 0 },
	{ "skip-hidden", 'S', NULL, 0, "Skip hidden files (leading '.' in file name)", 0 },
	{ NULL, 0, NULL, 0, "General options", 3 },
	{ "full", 'f', NULL, 0, "Ignore index file and make a full backup", 0 },
	{ "yes", 'y', NULL, 0, "Answer all questions with yes (script-friendly)", 0 },
	{ NULL, 0, NULL, 0, NULL, 0 }
};
struct arguments arguments = { 0, 0, NULL, NULL, NULL };

static error_t arguments_parse_opt(int key, char *arg, struct argp_state *state)
{
	// Get the input argument from argp_parse, which we know is a pointer to our arguments structure.
	struct arguments *arguments = state->input;
	
	switch(key)
	{
		case 'e':
		{
			path_exclude_pattern_add(arg);
			break;
		}
		case 'S':
		{
			path_skip_hidden_files(1);
			break;
		}
		case 's':
		{
			arguments->source = strdup(arg);
			break;
		}
		case 'a':
		{
			arguments->archive = strdup(arg);
			break;
		}
		case 'i':
		{
			arguments->index = strdup(arg);
			break;
		}
		case 'f':
		{
			arguments->full = 1;
			break;
		}
		case 'y':
		{
			arguments->yes = 1;
			break;
		}
		case ARGP_KEY_END:
		{
			if(arguments->source == NULL || arguments->archive == NULL || arguments->index == NULL)
			{
				argp_usage(state);
			}
			
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

void arguments_cleanup(void)
{
	free(arguments.source);
	free(arguments.archive);
}