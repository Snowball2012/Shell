/* Shell part 1, made by Snowball 27.09.2012 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "execution.h"
#include "parser.h"



void invite()
{
	char buf[256];
	getcwd(buf, sizeof(buf));
	printf("[%s] (,;,;,) ", buf);
}



/* MAIN --------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	struct argument * argList;
	int error;
	struct execNode * execList = NULL, * runList = NULL;
	printf("\nWelcome to Cthulhu sHEll.\nPh'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.\n\n");
	do {
		struct execNode * backProc = NULL;
		int argc, i;
		invite();
		argList = NULL;
		error = ParseString(&argList);
		if (!error) {
			int zombie;
			execList = List2arg(argList, execList);
			Execution(execList);
			while ((zombie = waitpid(-1, NULL, WNOHANG)) > 0)
				TerminatePid(zombie, runList);
		}
		if (error == 1)
			break;
		if (error == 2)
			printf("Mismatched quotes\n");
		while (execList) {
			struct execNode * tmp = execList->next;
			if(execList->status != RUNNING) {
				/*free((execList->argv)[argc]);*/
				while(execList->argc) {
					free((execList->argv)[execList->argc-1]);
					(execList->argc)--;
				}
				free(execList->argv);
				free(execList);
			} else {
				execList->next = runList;
				runList = execList;
			}
			execList = tmp;
		}
		execList = backProc;
	} while (error != 1);
	printf("\n");
	return 0;
}
