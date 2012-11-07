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
	printf("[%s] >> ", buf);
}



/* MAIN --------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	struct argument * argList;
	int error;
	do {
		int argc, i;
		argc = 0;
		char ** argv = NULL;
		invite();
		argList = NULL;
		error = ParseString(&argList);
		if (!error) {
			int pid;
			argv= List2arg(argList,&argc);
			pid = Execution(argv[0], argv);
		}
		if (error == 1)
			break;
		if (error == 2)
			printf("Mismatched quotes\n");
		for(i=0;i<=argc;i++)
			   free(argv[argc-i]);
		free(argv);	
	} while (error != 1);
	printf("\n");
	return 0;
}
