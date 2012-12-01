#ifndef CD_H
#define CD_H

#define THERE_IS_NO_CD 4
#define CD_NO_ARGS 3
#define CD_NO_ERROR 0
#define CD_ERROR -1
#define CD_MANY_ARGS 1
#define CD_CONVAYOR 2

void CDErrHandle(int error);

int CheckCD(char ** argv, char chk_only);

#endif
