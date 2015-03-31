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
#include <errno.h>
#include <utime.h> // utimes
#include <sys/time.h> // utimes

#include "walk.h"
#include "processor.h"
#include "path_helper.h"
#include "archive.h"
#include "arguments.h"

static error_t arguments_parse_opt(int key, char *arg, struct argp_state *state);

const char *argp_program_version = "backup 0.0.6-beta";
const char *argp_program_bug_address = "https://github.com/NIPE-SYSTEMS/incremental-backup/issues";
static const char *doc = "Incremental backup with tar-archives written in C.";
static struct argp_option options[] =
{
	{ NULL, 0, NULL, 0, "I/O options", 1 },
	{ "source", 's', "PATH", 0, "Path to the directory which should be stored in backup (REQUIRED)", 0 },
	{ "archive", 'a', "FILE", 0, "Path to the generated archive, default: 'backup.tar'", 0 },
	{ "index", 'i', "FILE", 0, "Path to the index directory. This path must exist. (REQUIRED)", 0 },
	{ "compress", 'c', "ALGORITHM", 0, "Possible ALGORITHM values: 'b64encode', 'bzip2', 'compress', 'grzip', 'gzip', 'lrzip', 'lzip', 'lzop', 'uuencode', 'xz'", 0 },
	{ NULL, 0, NULL, 0, "Recursivly directory walking options", 2 },
	{ "exclude", 'e', "PATTERN", 0, "Exclude files and directories which match the shell wildcard pattern PATTERN", 0 },
	{ "skip-hidden", 'S', NULL, 0, "Skip hidden files (leading '.' in file name)", 0 },
	{ NULL, 0, NULL, 0, "General options", 3 },
	{ "full", 'f', NULL, 0, "Ignore index and make a full backup", 0 },
	{ "no-output", 'n', NULL, 0, "Do not output status informations (script-friendly, parse-friendly)", 0 },
	{ "verbose", 'v', NULL, 0, "Verbose output", 0 },
	{ "stats", 't', NULL, 0, "Show stats at the end", 0 },
	{ NULL, 0, NULL, 0, NULL, 0 }
};
struct arguments arguments = { 0, 0, NULL, NULL, NULL, ARGUMENTS_ALG_NONE, 0, 0 };

/**
 * Callback: parses the command line options (see argp.h)
 */
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
			free(arguments->source);
			arguments->source = strdup(arg);
			break;
		}
		case 'a':
		{
			free(arguments->archive);
			arguments->archive = strdup(arg);
			break;
		}
		case 'i':
		{
			free(arguments->index);
			arguments->index = strdup(arg);
			break;
		}
		case 'f':
		{
			arguments->full = 1;
			break;
		}
		case 't':
		{
			arguments->show_stats = 1;
			break;
		}
		case 'n':
		{
			arguments->no_output = 1;
			break;
		}
		case 'v':
		{
			arguments->verbose = 1;
			break;
		}
		case 'c':
		{
			if(!strcmp(arg, "b64encode"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_B64ENCODE;
				break;
			}
			if(!strcmp(arg, "bzip2"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_BZIP2;
				break;
			}
			if(!strcmp(arg, "compress"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_COMPRESS;
				break;
			}
			if(!strcmp(arg, "grzip"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_GRZIP;
				break;
			}
			if(!strcmp(arg, "gzip"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_GZIP;
				break;
			}
			if(!strcmp(arg, "lrzip"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_LRZIP;
				break;
			}
			if(!strcmp(arg, "lzip"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_LZIP;
				break;
			}
			if(!strcmp(arg, "lzop"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_LZOP;
				break;
			}
			if(!strcmp(arg, "uuencode"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_UUENCODE;
				break;
			}
			if(!strcmp(arg, "xz"))
			{
				arguments->compression_algorithm = ARGUMENTS_ALG_XZ;
				break;
			}
			
			break;
		}
		case ARGP_KEY_END:
		{
			if(arguments->source == NULL || arguments->index == NULL)
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

/**
 * Parses command line options
 * @param argc amount of options
 * @param argv options as array
 */
void arguments_parse(int argc, char **argv)
{
	struct argp argp = { options, arguments_parse_opt, 0, doc, 0, 0, 0 };
	
	arguments.archive = strdup("backup.tar");
	
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
}

/**
 * Cleans all allocated memory from option parsing
 */
void arguments_cleanup(void)
{
	free(arguments.source);
	free(arguments.archive);
}