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
#include "convayor.h"


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
	if (!strcmp(list->word, "|"))
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



/* convert argument list to execution nodes list */
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
			if (status!=SEPARATOR && status!=BACKGROUND && status!=CONVAYOR) {
				printf("chell: empty command\n");
				break;
			}
			list = list->next;
		}
	}
	node = RollExeList(node);
	return node;
}



int ExecProc(struct execNode * list)
{
	int error;
	dup2(list->input, 0);
	dup2(list->output, 1);
	if(list->input!=0)
		close(list->input);
	if(list->output!=1)
		close(list->output);
	error = execvp(list->argv[0], list->argv);
	if (error) 
		perror("execvp");	
	exit(1);
	return 0;
}


int Execution(struct execNode * list)
{
	while (list) {
		if (list->argc >0) {
			/* Check for cd */
			int error;
			if (list->status == CONVAYOR) {
				/*process convayor, now empty*/
				error = Convayor(&list);
				if(error) {
					ConvErr(error);
					while(list) {
						if(list->input!=0)
							close(list->input);
						if(list->output!=1)
							close(list->output);
						list = list->next;
					}
					return 0;
				}
				continue;
			}
			error = CheckCD(list->argv, 0);
			if (error == THERE_IS_NO_CD) {
				list->pid = fork();
				if(!list->pid) 
					ExecProc(list);
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
