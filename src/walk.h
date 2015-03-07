int walk(char *path, void (*callback_process_files)(char *path), void (*callback_process_directories)(char *path));
int walk_get_error(void);
long long int walk_get_error_open_dir(void);
long long int walk_get_error_read_stat(void);