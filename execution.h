#ifndef EXECUTION_H
#define EXECUTION_H

#define TO_RUN 0
#define RUNNING 1
#define BACKGROUND 2
#define CONVAYOR 3
#define SEPARATOR 1
#define TERMINATED 4

struct argument {
	char * word;
	struct argument * next;
};

struct execNode {
	int argc;
	char ** argv;
	int status;
	int pid;
	struct execNode * next;
};


/* converts argument list to array for next execution */
struct execNode * List2arg(struct argument * list, struct execNode * node);
int Execution(struct execNode * list);

#endif
