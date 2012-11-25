#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "execution.h"
#include "cd.h"
#include "redirection.h"


/*TODO:fix empty command execution*/
/* Parses word and checks for special symbols, returns code, which described below */

int ParseWord(struct argument * list)
{
	if (list == NULL)
		return END_LIST;
	if (!strcmp(list->word, "&"))
		return BACKGROUND;
	if (!strcmp(list->word, ";"))
		return SEPARATOR;
	if (!strcmp(list->word, "||"))
		return CONVAYOR;
	if (!strcmp(list->word, "<") || !strcmp(list->word, ">") || !strcmp(list->word, ">>"))
		return REDIRECTION;	
	return SIMPLE_WORD;
}

struct execNode * RollExeList(struct execNode * list)
{
	struct execNode *rolled = NULL, * temp;
	while(list) {
		temp = rolled;
		rolled = list;
		list = list->next;
		rolled->next = temp;
	}
	return rolled;
}

void FreeExec(struct execNode * list) 
{
	struct execNode * tmp;
	while (list) {
		int i;
		tmp = list->next;
		for(i = list->argc - 1; i>=0; i--) 
			free((list->argv)[i]);
		free(list->argv);
		if(list->input != 0) 
			close(list->input);
		if(list->output != 1)
			close(list->output);
		free(list);
		list = tmp;
	}
}



/* convert argument list to argv[]
 * status says if there were &, |, ; or redirection symbols 
 * 0 - nothing
 * 1 - ;
 * 2 - &
 * 3 - |
 * 4 - redirection */
struct execNode * List2arg(struct argument * list)
{
	struct execNode * node = NULL;
	int argc = 0;
	char ** argv = NULL;
	int status;
	int i, error;
	while (list) {
		struct argument * tmpArg = list;
		argc = 0;
		while ((status = ParseWord(list)) == SIMPLE_WORD) {
			argc++;
			list = list->next;
		}
		if (argc>0) {
			struct execNode * tmpNode = node;
			list = tmpArg;
			node = (struct execNode *)malloc(sizeof(*node));
			argv = (char **)malloc(argc*sizeof(char *)+1);
			node->argc = argc;
			node->input = 0;
			node->output = 1;
			node->status = status;
			for (i = 0; i<argc; i++) {
				argv[i] = list->word;
				list = list->next;
			}
			argv[argc] = NULL;
			node->argv = argv;
			node->next = tmpNode;
			if (status == REDIRECTION) {
				error = Redirection(&list, node);
				if (error) {
					RedirError(error, node);
					FreeExec(node);
					return NULL;
				}
			}
		} else {
			if (status!=SEPARATOR && status!=BACKGROUND) {
				printf("chell: empty command\n");
				break;
			}
			list = list->next;
		}
	}
	node = RollExeList(node);
	return node;
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


int Execution(struct execNode * list)
{
	while (list) {
		if (list->argc >0) {
			/* Check for cd */
			int error;
			if (list->status == CONVAYOR) {
				/*process convayor, now empty*/
				list = list->next;
				continue;
			}
			error = CheckCD(list->argv);
			if (error == THERE_IS_NO_CD) {
				list->pid = fork();
				if(!list->pid) {
					dup2(list->input, 0);
					dup2(list->output, 1);
					if(list->input!=0)
						close(list->input);
					if(list->output!=1)
						close(list->output);
					error = execvp(list->argv[0], list->argv);
					if (error) {
						ExecErrHandle(errno, list->argv);
						exit(1);
					}
				}
				if (list->input!=0)
					close(list->input);
				if (list->output!=1)
					close(list->output);
				if (list->status != BACKGROUND) {
					int ch_pid;
					while(list->pid != (ch_pid = wait(NULL)))
						printf("chell: [%i] terminated\n", ch_pid);
				} else {
					printf("PID:[%i]\n", list->pid);
				}
			}
		}
		list = list->next;
	}
	return 0;
}
