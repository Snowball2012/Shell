/* Shell part 1, made by Snowball 27.09.2012 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
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
	struct argument * argList = NULL;
	int error;
	struct execNode * execList = NULL;
	printf("\nWelcome to Cthulhu sHEll.\nPh'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.\n\n");
	do {
		execList = NULL;
		argList = NULL;
		invite();
		error = ParseString(&argList);
		if (!error) {
			int zombie;
			execList = List2arg(argList);
			Execution(execList);
			while ((zombie = waitpid(-1, NULL, WNOHANG)) > 0)
				printf("chell: [%i] terminated\n", zombie);
		}
		if (error == 1)
			break;
		if (error == 2)
			printf("Mismatched quotes\n");
		FreeExec(execList);
		while (argList) {
			struct argument * tmpArg = argList;
			argList = argList->next;
			free(tmpArg);
		}
	} while (error != 1);
	printf("\n");
	return 0;
}
