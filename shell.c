/* Shell part 1, made by Snowball 27.09.2012 */
#include <stdio.h>
#include <stdlib.h>
#include "execution.h"
#define STD_BUFF_SIZE 128


/*buffer structure*/
struct buffer {
	char * content;
	int place;
	int size;
};



void invite()
{
	printf(" >> ");
}


/* Add new word from buffer to the list, wordEnd - position of \0 in the buffer -----------------------------*/
void AddWord(struct buffer buff, struct argument ** argList)
{
	int wordEnd = buff.place;
	if (wordEnd==0)
		return;
	(buff.content)[wordEnd] = '\0';
	char * word = (char*)malloc(sizeof(char)*(wordEnd+1));
	char * wordStart = word;
	while (*buff.content!='\0'){
		*word = *buff.content;
		word++;
		(buff.content)++;
	}
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
			default:
				AddChar(c, &buff);
				break;
		}
	} while (!isEnd && !error);
	if(!error) 
		AddWord(buff, argList);
	free(buff.content);
	return error;
}

void PrintArgs(struct argument * args)
{
	struct argument * tmpArg = args;
	while (tmpArg) {
		int i = 0;
		putchar('[');
		while (tmpArg->word[i]){
			putchar(tmpArg->word[i]);
			i++;
		}
		putchar(']');
		tmpArg = tmpArg->next;
	}
	if (args)
		printf("\n");
}

/* MAIN --------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	struct argument * argList;
	int error;
	do {
		struct argument * tmpArg;
		invite();
		argList = NULL;
		error = ParseString(&argList);
		if (!error) 
			PrintArgs(argList);
		if (error == 2)
			printf("Mismatched quotes\n");
		tmpArg = argList;
		while(tmpArg) {
			free(tmpArg->word);
			argList = tmpArg->next;
			free(tmpArg);
			tmpArg = argList;
		}
	} while (error != 1);
	printf("\n");
	return 0;
}
