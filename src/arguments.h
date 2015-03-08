struct arguments
{
	int number;
	char *source;
	char *archive;
	char *index;
};
extern struct arguments arguments;

void arguments_parse(int argc, char **argv);
void arguments_cleanup(void);