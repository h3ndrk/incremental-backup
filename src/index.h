typedef struct
{
	long long int timestamp;
	char *path;
	char flag;
} index_file;

int index_add(long long int timestamp, char *path, char flag);
void index_cleanup(void);