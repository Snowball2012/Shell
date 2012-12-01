#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include "convayor.h"
#include "cd.h"

enum {
	not_enough_commands = 1,
	redirection_in_conv,
	cd_error,
	pipe_creation_error,
};

int Convayor(struct execNode ** list)
{
	struct execNode * start, * tmp, ** nodes;
	int count = 0;
	int i, *fds, pid, j;
	start = *list;
	while (*list) {
		count++;
		if ((*list)->status!=CONVAYOR) {
			break;
		}
		*list = (*list)->next;
	}
	if (*list)
		*list = (*list)->next;
	if (count <= 1) {
		*list = start;
		return not_enough_commands;
	}
	/*check for errors with cd or redirection*/
	tmp = start;
	if (tmp->output != 1) {
		*list = start;
		return redirection_in_conv;
	}
	if (CheckCD(tmp->argv, 1) != THERE_IS_NO_CD) {
		*list = start;
		return cd_error;
	}
	tmp = tmp->next;	
	for (i = 2; i<count; i++) {
		if (tmp->input != 0 || tmp->output != 1) {
			*list = start;
			return redirection_in_conv;
		}
		if(CheckCD(tmp->argv, 1) != THERE_IS_NO_CD) {
			*list = start;
			return cd_error;
		}
		tmp = tmp->next;
	}
	if (tmp->input != 0) {
		*list = start;
		return redirection_in_conv;
	}
	if (CheckCD(tmp->argv, 1) != THERE_IS_NO_CD) {
		*list = start;
		return cd_error;
	}
	/* everything is ok, time to create pipes*/
	/* create array for execNodes and array for fd's */
	nodes = (struct execNode **)malloc(count*sizeof(*nodes));
	fds = (int *)malloc(count*2*sizeof(int));
	fds[count*2-1] = tmp->output;
	nodes[count-1] = tmp;
	tmp = start;
	fds[0] = tmp->input;
	for (i = 1; i<count; i++) {
		int tempFD;
		nodes[i-1] = tmp;
		if(pipe(fds+i*2-1) == -1) {
			*list = start;
			return pipe_creation_error;
		}
		tempFD = fds[i*2-1];
		fds[i*2-1] = fds[i*2];
		fds[i*2] = tempFD;
		tmp = tmp->next;
	}
	for (i = 0; i<count; i++) {
		nodes[i]->input = fds[i*2];
		nodes[i]->output = fds[i*2+1];
	}
	/* pipes created */
	/* time to fork ! */
	/* just a little magic */
	for (i = 0; i<count; i++)
		if((pid = fork()) == 0)
			break;
	/* now i contains index of process to execute */
	/* close all unnecesary fd's */
	if (pid == 0) {
		/* child process */
		for (j = 0; j<count; j++) {
			if (j!=i) {
				if(nodes[j]->input != 0)
					close(nodes[j]->input);
				if(nodes[j]->output != 1)
					close(nodes[j]->output);
			}
		}
		/* execute */
		ExecProc(nodes[i]);
	}
	/* parent process */
	for (i = 1; i<count; i++) {
		close(fds[i*2-1]);
		close(fds[i*2]);
	}
	if (nodes[count-1]->status != BACKGROUND) {
		int ch_pid;
		while (pid != (ch_pid = wait(NULL)));
	} else {
		printf("PID:[%i]\n", pid);
	}
	free (nodes);
	free (fds);
	return 0;
}

void ConvErr(int error)
{
	//do smth
	switch (error) {
		case not_enough_commands:
			printf("chell: not enough commands for convayor\n");
			break;
		case redirection_in_conv:
			printf("chell: duplicate input/output streams\n");
			break;
		case cd_error:
			printf("chell: cd in convayor\n");
			break;
		case pipe_creation_error:
			perror("pipe");
			break;
		default: break;
	}
}
