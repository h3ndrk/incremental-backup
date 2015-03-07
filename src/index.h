typedef struct
{
	long long int timestamp;
	char *path;
	long long int path_length;
	char flag;
} index_file;

int index_files_add(long long int timestamp, char *path, char flag);
void index_files_cleanup(void);
long long int index_files_get_timestamp_by_path(char *path, long long int path_length);
long long int index_files_get_timestamp_by_index(long long int index);
long long int index_files_get_amount(void);
int index_process_file(char *path, void (*callback_index_process_file)(long long int timestamp, char *path));
int index_saved_add(long long int timestamp, char *path, char flag);
void index_saved_cleanup(void);
long long int index_saved_get_timestamp_by_path(char *path, long long int path_length);
long long int index_saved_get_timestamp_by_index(long long int index);
long long int index_saved_get_amount(void);
void index_compare_files_with_index(void);
int index_write_saved(char *path);