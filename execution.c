#include "execution.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

char ** List2arg(struct argument * list, int * argc)
{
	char ** argv;
	int i;
	struct argument * tmpArg = list;
	for (*argc=0; tmpArg!= NULL; (*argc)++) 
		tmpArg = tmpArg->next;
	argv = malloc(sizeof(char*)*(*argc+1));
	tmpArg = list;
	for (i = 1; i<=*argc; i++) {
		argv[*argc-i] = tmpArg->word;
		tmpArg = tmpArg->next;
		free(list);
		list = tmpArg;
	}
	argv[*argc] = NULL;
	return argv;
}

int Execution(char * filename, char ** argv)
{
		int pid;
		int status;
		pid = fork();
		if(!pid) {
			execvp(filename, argv);
		}
		wait(&status);
		return pid;
}









