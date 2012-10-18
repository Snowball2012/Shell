#ifndef EXECUTION_H
#define EXECUTION_H

struct argument {
	char * word;
	struct argument * next;
};

/* converts argument list to array for next execution */
char ** List2argv(struct argument * list, int * argc);

#endif
