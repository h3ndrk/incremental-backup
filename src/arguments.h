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
};
extern struct arguments arguments;

void arguments_parse(int argc, char **argv);
void arguments_cleanup(void);