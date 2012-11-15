#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "execution.h"
#include "cd.h"



/* Parses word and checks for special symbols, returns code, which described below */

int ParseWord(char * word)
{
	if (!strcmp(word, "&"))
		return BACKGROUND;
	if (!strcmp(word, ";"))
		return SEPARATOR;
	if (!strcmp(word, "||"))
		return CONVAYOR;
	return TO_RUN;
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



/* convert argument list to argv[]
 * status says if there were &, ||, ; or redirection symbols 
 * 0 - nothing
 * 1 - ;
 * 2 - &
 * 3 - ||
 * 4 - redirection */

struct execNode * List2arg(struct argument * list, struct execNode * node)
{
	char ** argv;
	int i;
	struct argument * tmpArg = list;
	while(tmpArg) {
		int argc;
		struct execNode * tmp = node;
		node = (struct execNode *)malloc(sizeof(*node));
		for (argc=0; tmpArg!= NULL && (node->status = ParseWord(tmpArg->word)) == 0; argc++) 
			tmpArg = tmpArg->next;
		argv = (char **)malloc(sizeof(char*)*(argc+1));
		tmpArg = list;
		for (i = 0; i<argc; i++) {
			argv[i] = tmpArg->word;
			tmpArg = tmpArg->next;
			free(list);
			list = tmpArg;
		}
		if (list) {
			list = list->next;
			free(tmpArg);
			tmpArg = list;
		}
		argv[argc] = NULL;

		node->next = tmp;
		node->argc = argc;
		node->argv = argv;
		node->pid = 0;
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

void TerminatePid(int pid, struct execNode * list) 
{
	while(list->pid != pid && list->next!=NULL)
		list = list->next;
	if(list->pid == pid) {
		printf("chell: process with pid [%i] terminated\n", pid); 
		list->status = TERMINATED;
	}
}

int Execution(struct execNode * list)
{
	struct execNode * start = list;
	while (list) {
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
				error = execvp(list->argv[0], list->argv);
				if (error) {
					ExecErrHandle(errno, list->argv);
					exit(1);
				}
			}
			if (list->status != BACKGROUND) {
				int ch_pid;
				while(list->pid != (ch_pid = wait(NULL)))
					TerminatePid(ch_pid, start);
			} else {
				printf("PID:[%i]\n", list->pid);
				list->status = RUNNING;
			}
		}
		list = list->next;
	}
	return 0;
}
