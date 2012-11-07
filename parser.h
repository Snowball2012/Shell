#ifndef PARSER_H
#define PARSER_H

#include "execution.h"
#define STD_BUFF_SIZE 128


/*buffer structure*/
struct buffer {
	char * content;
	int place;
	int size;
};

/* Parser functions prototypes */

void AddWord(struct buffer buff, struct argument ** argList);

struct buffer InitBuff(void);

void ExpandBuff(struct buffer * buff);

void AddChar(const char c, struct buffer * buff);

void ProcessSpace(struct buffer * buff, char quotes, struct argument ** argList);

int ParseString(struct argument ** argList);

#endif
