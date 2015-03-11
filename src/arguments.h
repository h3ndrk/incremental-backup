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

#define ARGUMENTS_ALG_B64ENCODE 0
#define ARGUMENTS_ALG_BZIP2 1
#define ARGUMENTS_ALG_COMPRESS 2
#define ARGUMENTS_ALG_GRZIP 3
#define ARGUMENTS_ALG_GZIP 4
#define ARGUMENTS_ALG_LRZIP 5
#define ARGUMENTS_ALG_LZIP 6
#define ARGUMENTS_ALG_LZOP 7
#define ARGUMENTS_ALG_NONE 8
#define ARGUMENTS_ALG_UUENCODE 9
#define ARGUMENTS_ALG_XZ 10

struct arguments
{
	char full;
	char yes;
	char *source;
	char *archive;
	char *index;
	char compression_algorithm;
	char verbose;
};
extern struct arguments arguments;

void arguments_parse(int argc, char **argv);
void arguments_cleanup(void);