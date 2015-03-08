struct arguments
{
	int number;
	char *source;
	char *archive;
};
extern struct arguments arguments;

void arguments_parse(int argc, char **argv);
void arguments_cleanup(void);