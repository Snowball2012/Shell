#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include "redirection.h"
#include "execution.h"

/* Redirection handler, error codes:
 * 0 - All are fine
 * 1 - too many args
 * 2 - no args
 * 3 - error in errno
 */

int Redirection(struct argument ** list, struct execNode * node) 
{
	int inpFlag = 0;
	int outFlag = 0;
	int fileFlags;
	while (*list) {
		fileFlags = 0;
		char * redsymb;
		int fd;
		int type;
		int * destination;
		type  = ParseWord(*list);
		switch (type) {
			case SIMPLE_WORD:
				return 1;
			case BACKGROUND:
			case CONVAYOR:
			case SEPARATOR:
				node->status = type;
				(*list) = (*list)->next;
				return 0;
			default:
				break;
		}
		redsymb = (*list)->word;
		*list = (*list)->next;
		if (ParseWord(*list)!=SIMPLE_WORD)
			return 2;
		if (!strcmp(redsymb, ">>")) {
			if (outFlag != 0)
				return 1;
			outFlag++;
			fileFlags = O_APPEND|O_CREAT|O_WRONLY;
			destination = &(node->output);
		}
		if (!strcmp(redsymb, ">")) {
			if (outFlag != 0)
				return 1;
			outFlag++;
			fileFlags = O_CREAT|O_WRONLY|O_TRUNC;
			destination = &(node->output);
		}
		if (!strcmp(redsymb, "<")) {
			if (inpFlag != 0)
				return 1;
			inpFlag++;
			destination = &(node->input);
			fileFlags = O_RDONLY;
		}
		fd = open((*list)->word, fileFlags, 0664); /*if new file created, permissions set to rw--rw--r--*/
		if (fd == -1) 
			return 3;
		*destination = fd;
		(*list) = (*list)->next;
	}		
	return 0;
}

void RedirError(int error, struct execNode * execList) 
{
	switch (error) {
		case 1:
			printf("chell: too many args in redirection\n");
			break;
		case 2:
			printf("chell: no args in redirection\n");
			break;
		case 3:
			printf("chell: unable to open file. Reason: ");
			switch (errno) {
				case EACCES: 
					printf("You don't have enough permissions\n");
					break;
				case EISDIR:
					printf("unable to write into directory\n");
					break;
				case ENFILE:
					printf("too many opened files in system\n");
					break;
				case ENOENT:
					printf("file doesn't exists\n");
					break;
				default:
					printf("unknown error\n");
					break;
			}
			break;
		default:
			break;
	}
}
