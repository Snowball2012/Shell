#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "execution.h"
#include "cd.h"

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

void ExecErrHandle(int error,char ** argv)
{
	switch(error) {
		case EPERM:
			printf("Operation not permitted\n");
			break;
		case ENOENT:
			printf("[%s]: Command not found\n", argv[0]);
			break;
		case E2BIG:
			printf("Argument list too long\n");
			break;
		case ENOEXEC:
			printf("Exec format error\n");
			break;
		case ENOMEM:
			printf("Cannot allocate memory\n");
			break;
		default:
			printf("Unknown error");
	}
}

int Execution(char * filename, char ** argv)
{
	/* Check for cd */
	int error;
	int pid;
	int status;
	error = CheckCD(argv);
	if (error == 4) {
		pid = fork();
		if(!pid) {
			error = execvp(filename, argv);
			if (error) {
				ExecErrHandle(errno, argv);
				exit(1);
			}
		}
		wait(&status);
		return pid;
	} else
		return 0;
}









