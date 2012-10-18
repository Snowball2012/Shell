#include "execution.h"
#include <stdlib.h>

char ** List2argist, int * argc)
{
	char ** argv;
	int i;
	struct argument * tmpArg = list;
	for (*argc=0; tmpArg!= NULL; (*argc)++) 
		tmpArg = tmpArg->next;
	argv = malloc(sizeof(char*)*(*argc));
	tmpArg = list;
	for (i = 1; i<=*argc; i++) {
		argv[*argc-i] = tmpArg->word;
		tmpArg = tmpArg->next;
		free(list);
		list = tmpArg;
	}
	return argv;
}
