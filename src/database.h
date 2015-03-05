int database_open(void);
void database_close(void);
int database_file_exists(char *path);
int database_files_flag(void);
int database_file_unflag(char *path);
int database_files_delete_flagged(void);