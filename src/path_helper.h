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

char *concatenate_paths(char *prefix, char *suffix);
int compare_paths(char *path0, long int path0_length, char *path1, long int path1_length);
int path_exclude_pattern_add(char *pattern);
void path_exclude_pattern_print(void);
void path_exclude_pattern_cleanup(void);
int path_exclude_pattern_match(char *path);
void path_skip_hidden_files(char state);
int path_get_skip_hidden_files(void);
int path_compare_timestamps(long int tv_sec0, long int tv_sec1);
int path_mkpath(char *file_path, mode_t mode);