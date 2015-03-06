#define ARCHIVE_BUFF_SIZE 16384

int archive_open(char *path);
int archive_add_file(char *path);
void archive_close(void);