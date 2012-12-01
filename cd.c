/* Cd command handler */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "execution.h"
#include "cd.h"


/* Error codes:
 * 0 - OK
 * 1 - Too many args
 * 2 - cd in convayor
 * 3 - no args 
 * 4 - there is no cd
 */

/* Handler for run-time chdir errors */


/* Checking argv for errors if command is cd */

int ErrCD(char ** argv)
{
	int argc = 0;
	while (argv[argc] != NULL)
		argc++;
	if (argc > 2)
		return CD_MANY_ARGS;
	if (argc <=1)
		return CD_NO_ARGS;
	return CD_NO_ERROR;
}



/* CD command handler , 2-nd arg is a flag for check only */

int CheckCD(char ** argv, char chk_only)
{
	int error = 0;
	if(!strcmp(argv[0], "cd")) {
		error = ErrCD(argv);
		if(!error) 	
			error = chdir(argv[1]);
		if(error == -1) 
			perror("cd");	
		else if(error) {
			switch (error) {
				case CD_MANY_ARGS:
					printf(" cd: Too many arguments\n");
					break;
				case CD_NO_ARGS:
					printf(" cd: No arguments\n");
					break;
				case CD_CONVAYOR:
					printf(" cd: Cannot be used in the Convayor\n");
					break;
				default:
					break;
			}
		}
	} else 
		return THERE_IS_NO_CD;
	return error;
}



