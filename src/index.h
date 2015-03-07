typedef struct
{
	long long int timestamp;
	char *path;
	char flag;
} index_file;

int index_add(long long int timestamp, char *path, char flag);
void index_cleanup(void);
void index_print(void);
long long int index_get_timestamp_by_path(char *path);
long long int index_get_amount(void);
int index_process_file(char *path, void (*callback_index_process_file)(long long int timestamp, char *path));