#include <stdio.h>
#include <stdlib.h>
#include "parser.h"




/* Add new word from buffer to the list, wordEnd - position of \0 in the buffer -----------------------------*/
void AddWord(struct buffer buff, struct argument ** argList)
{
	int wordEnd = buff.place;
	if (wordEnd==0)
		return;
	(buff.content)[wordEnd] = '\0';
	char * word = (char*)malloc(sizeof(char)*(wordEnd+1));
	char * wordStart = word;
	char * content = buff.content;
	while (*buff.content!='\0'){
		*word = *buff.content;
		word++;
		(buff.content)++;
	}
	buff.content = content;
	*word = '\0';	
	struct argument * newItem = (struct argument *)malloc(sizeof(*newItem));
	newItem->next = *argList;
	newItem->word = wordStart;
	*argList = newItem;
}

/* buffer functions */
struct buffer InitBuff(){
	struct buffer buff;
	buff.content = (char*)malloc(STD_BUFF_SIZE*sizeof(char));
	buff.place = 0;
	buff.size = STD_BUFF_SIZE;
	return buff;
}

/* Expand buffer by STD_BUFF_SIZE */
void ExpandBuff(struct buffer * buff)
{
	char * newBuff;
	int n;
	newBuff = (char*)malloc(sizeof(char)*(STD_BUFF_SIZE+buff->size));
	for (n=0; n<buff->size; n++)
		newBuff[n]=(buff->content)[n];
	free(buff->content);
	buff->content = newBuff;
	buff->size += STD_BUFF_SIZE;
}


/*Add letter to buffer*/
void AddChar(const char c, struct buffer * buff)
{
	char * content = buff->content;
	int * place = &(buff->place);
	int * size = &(buff->size);
	content[*place] = c;
	(*place)++;
	if(*place>*size-1)
		ExpandBuff(buff);
}

void ProcessSpace(struct buffer * buff, char quotes, struct argument ** argList)
{
	if(quotes){
		AddChar(' ', buff);
	} else {
		AddWord(*buff, argList);
		buff->place = 0;
	}
}

struct argument * RollList(struct argument * list) 
{
	struct argument *rolled = NULL, * temp;
	while(list) {
		temp = rolled;
		rolled = list;
		list = list->next;
		rolled->next = temp;
	}
	return rolled;
}


/*Parses new string in the command line---------------------------------------------------------------------
 * error codes:
 * 1 - EOF, program will be terminated
 * 2 - mismatched quotes
 * -------------------------------------------------------------------------------------------------------*/
int ParseString(struct argument ** argList)
{
	char quotes = 0; /*quotes flag, 0 = no open quotes*/
	struct buffer buff = InitBuff();
	int error=0;
	int isEnd = 0;   /*line end flag */
	int outRedir = 0; /*to differ > and >> */
	do {
		int c = getchar();
		switch(c) {
			case EOF:
				error = 1;
				break;
			case '\n':
				if(quotes) 
					error = 2;
				else 
					isEnd = 1;
				break;
			case ' ':
				ProcessSpace(&buff, quotes, argList);
				break;
			case '\"':
				quotes = !quotes;
				break;
			case '&':
			case ';':
			case '<':
			case '|':
				if(!quotes){
					AddWord(buff, argList);
					buff.place = 0;
					AddChar(c, &buff);
					AddWord(buff, argList);
					buff.place = 0;
				} else {
					AddChar(c, &buff);
				}
				break;
			case '>':	
				if(!quotes) {
					if(outRedir) { 
						outRedir = 0;
						AddChar(c, &buff);
						AddWord(buff, argList);
						buff.place = 0;
					} else {
						outRedir = 1;
						AddWord(buff, argList);
						buff.place = 0;
						AddChar(c, &buff);
					}
				} else {
					AddChar(c, &buff);
				}
				break;
			default:
				if(outRedir) {
					outRedir = 0;
					AddWord(buff, argList);
					buff.place = 0;
				}
				AddChar(c, &buff);
				break;
		}
	} while (!isEnd && !error);
	if(error==0) 
		AddWord(buff, argList);
	free(buff.content);
	*argList = RollList(*argList);
	return error;
}
