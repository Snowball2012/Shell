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
	struct execNode * execList = NULL;
	printf("\nWelcome to Cthulhu sHEll.\nPh'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.\n\n");
	do {
		int argc, i;
		invite();
		argList = NULL;
		error = ParseString(&argList);
		if (!error) {
			execList = List2arg(argList, execList);
			Execution(execList);
		}
		if (error == 1)
			break;
		if (error == 2)
			printf("Mismatched quotes\n");
		while (execList) {
			struct execNode * tmp = execList->next;
			if(execList->status == TERMINATED) {
				while(execList->argc) {
					free(execList->argv[argc-1]);
					(execList->argc)--;
				}
				free(execList->argv);
				free(execList);
				execList = tmp;
			}
		}
	} while (error != 1);
	printf("\n");
	return 0;
}
