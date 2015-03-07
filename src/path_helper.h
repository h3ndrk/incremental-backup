char *concatenate_paths(char *prefix, char *suffix);
int compare_paths(char *path0, long long int path0_length, char *path1, long long int path1_length);
int path_exclude_pattern_add(char *pattern);
void path_exclude_pattern_cleanup(void);
int path_exclude_pattern_match(char *path);