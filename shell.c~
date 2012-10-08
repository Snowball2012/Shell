/* Shell part 1, made by Snowball 27.09.2012 */
/* TODO:fix issues with word processing*/
#include <stdio.h>
#include <stdlib.h>
#define STD_BUFF_SIZE 128


/*item from list of the args*/
struct argument {
	char * word;
	struct argument * next;
};


void invite()
{
	printf("\n >> ");
}


/* Add new word from buffer to the list, wordEnd - position of \0 in the buffer -----------------------------*/
void AddWord(char * buff, int wordEnd, struct argument ** argList) 
{
	printf("\nwe entered AddWord\n"); /*DEBUG*/
	char * word = (char*)malloc(sizeof(char)*(wordEnd+1));
	printf("\n malloc finished\n, wordEnd = %i", wordEnd);
	char * wordStart = word;
	printf("\n%s\n",buff);
	while (*word++ = *buff++); /*{
		printf("\n maybe... \n");
		*word = *buff;
		printf("\n%c\n",*buff);
		buff++;
		printf("\n%c\n",*buff);
		word++;
	}*/
	printf("\n%s\n",buff);
	*word = '\0';
	/*debug*/
	printf("we come so close... word copied");
	struct argument * newItem = (struct argument *)malloc(sizeof(*newItem));
	if (*argList){
		while ((*argList)->next) {};
		(*argList)->next = newItem;
	} else {
		*argList = newItem;
	}
	newItem->next = NULL;
	newItem->word = wordStart;
}
void ExpandBuff(char ** buff, int * buffSize)
{
	char * newBuff;
	int n;
	newBuff = (char*)malloc(sizeof(char)*(STD_BUFF_SIZE+*buffSize));
	for (n=0; n<*buffSize; n++) 
		newBuff[n]=(*buff)[n];
	free(*buff);
	*buff = newBuff;
	*buffSize += STD_BUFF_SIZE;
}
	
/*Parses new string in the command line---------------------------------------------------------------------
 * error codes:
 * 1 - EOF, program will be terminated
 * 2 - mismatched quotes
 * 3 - misplaced quotes
 * -------------------------------------------------------------------------------------------------------*/
int ParseString(struct argument ** argList)
{
	char quotes = 0; /*quotes flag, 0 = no open quotes*/
	char * buff = (char*)malloc(STD_BUFF_SIZE*sizeof(char));
	int place = 0; /*place of the last letter in buffer*/
	int buffSize = STD_BUFF_SIZE;
	int error=0;
	char isEnd=0; /*line end flag*/
	char waitForWord = 1;
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
				if(!waitForWord){
					if (quotes){
						buff[place] = ' ';
						place++;
						if(place>buffSize-1)
							ExpandBuff(&buff, &buffSize);
					} else {
						waitForWord = 1;
						buff[place] = '\0';
						AddWord(buff, place, argList);
						place = 0;
					}
				}
				break;
			case '\"':
				if(quotes){
					quotes = 0;
					waitForWord = 1;
					buff[place] = '\0';
					AddWord(buff, place, argList);
					place = 0;
				} else {
					if(waitForWord){
						quotes = 1;
						waitForWord = 0;
					} else {
						isEnd = 1;
						error = 3;
					}
				}
				break;
			default:
				waitForWord = 0;
				buff[place] = c;
				place++;
				if(place>buffSize-1)
					ExpandBuff(&buff, &buffSize);
				break;
		}
	} while (!isEnd && !error);
	if(error)
		return error;
	buff[place] = '\0';
	AddWord(buff, place, argList);
	return 0;
}












/* TODO: to complete main() --------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	struct argument * argList;
	int error;
	do {
		struct argument * tmpArg;
		invite();
		argList = NULL;
		error = ParseString(&argList);
		printf("\nParseString terminated\n"); /*DEBUG*/
		printf("\n");
		tmpArg = argList;
		if (!error) {
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
			putchar ('\n');
		}
		if (error == 2) 
			printf("Mismatched or misplaced quotes");
		if (error == 3)
			printf("Misplaced quotes");
		tmpArg = argList;
		while(tmpArg) {
				argList = tmpArg->next;
				free(tmpArg);
				tmpArg = argList;
		}				
	} while (error != 1);	
	printf("\n");
	return 0;
}
