#ifndef EXECUTION_H
#define EXECUTION_H

#define SIMPLE_WORD 0
#define RUNNING 5
#define BACKGROUND 2
#define CONVAYOR 3
#define SEPARATOR 1
#define REDIRECTION 4
#define END_LIST 5

struct argument {
	char * word;
	struct argument * next;
};

struct execNode {
	int argc;
	char ** argv;
	int status;
	int pid;
	int input;
	int output;
	struct execNode * next;
};


/* converts argument list to array for next execution */
struct execNode * List2arg(struct argument * list);
int Execution(struct execNode * list);
void TerminatePid(int pid, struct execNode * list);
void FreeExec(struct execNode * list);
int ParseWord(struct argument * list);

#endif
