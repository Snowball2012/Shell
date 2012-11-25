#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "execution.h"

int Redirection(struct argument ** list, struct execNode * execList);
void RedirError(int error, struct execNode * execList);

#endif
