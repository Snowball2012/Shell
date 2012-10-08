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

/* Write word from std istream to buffer, first letter is in buffer already */
int GetWord(char * buff, int * buffSize, char quotes)
{
	int i = 0;
	int c;
	do {
		i++;
		c=getchar();
		if (c == EOF)
			c = '\n';
		/*resize buffer if needed*/
		if (i>*buffSize) {
			char * newBuff;
			int n;
			newBuff = (char*)malloc(sizeof(char)*(STD_BUFF_SIZE+*buffSize));
			for (n=0; n<*buffSize; n++) 
				newBuff[n]=buff[n];
			free(buff);
			buff = newBuff;
			*buffSize += STD_BUFF_SIZE;
		}
		buff[i]=c;
	} while (c!='\n' && (c!=' '||quotes) && c!='\"'); 
	return i;
}

/*Checks word for errors and adds \0 to the end--------------------------------------------------------------*/
/* error codes
 * 0 - it's OK
 * 1 - unmatched quotes
 * 2 - end of the line or EOF
 * 3 - smth strange happened */
int CheckBuff(char * buff, int place, char * quotes) /*place - position of the last letter in buffer*/
{
	switch (buff[place]) {
		case ' ': 
			if (*quotes)
				return 1;
			break;
		case '\n': 
			if (*quotes) {
				return 1; 
			} else {
				buff[place] = '\0';
				return 2;
			}
			break;
		case '\"': 
			if (!(*quotes)) {
				return 1;
			} else {
				*quotes = 0;
			}
			break;
		default:
			return 3;
			break;
	}
	buff[place] = '\0';
	return 0;
}

/* Add new word from buffer to the list, wordEnd - position of \0 in the buffer -----------------------------*/
void AddWord(char * buff, int wordEnd, struct argument ** argList) 
{
	char * word = (char*)malloc(sizeof(char)*(wordEnd+1));
	char * wordStart = word;
	while (*buff) {
		*word = *buff;
		buff++;
		word++;
	}
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

/*Parses new string in the command line---------------------------------------------------------------------*/
int ParseString(struct argument ** argList)
{
	int c;
	char quotes = 0; /*quotes flag, 0 = no open quote yet*/
	char * buff = (char*)malloc(STD_BUFF_SIZE*sizeof(char));
	int buffSize = STD_BUFF_SIZE;
	while (c=getchar()){
		if (c == EOF || c == '\n')
			break;
		if (c == '\"') {
			quotes = !quotes;
		} else	
		if (c!=' '){      
			int error;
			int place;    /*position of the last char in buff*/
			buff[0] = c;
			printf("\nGetWord is running\n");   /*DEBUG*/
			place = GetWord(buff, &buffSize, quotes);
			printf("\nGetWord Terminated\n"); /*DEBUG*/
			error = CheckBuff(buff, place, &quotes);
			printf("\nCheckBuff Terminated\n"); /*DEBUG*/
			if (error!=2 && error!=0) {
				printf("error code: %i", error);	
				return error;
			}	
			AddWord(buff, place,argList); 
			printf("\nAddWord Terminated\n"); /*DEBUG*/
			if (error == 2)
				return 0;
			quotes = 0;
		}
	}
	free(buff);
	return 0;
}
/* TODO: to complete main() --------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
	struct argument * argList;
	do {
		int error;
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
		if (error == 1) 
			printf("Mismatched or misplaced quotes");
		if (error == 2)
			printf("something strange happened");
		tmpArg = argList;
		while(tmpArg) {
				argList = tmpArg->next;
				free(tmpArg);
				tmpArg = argList;
		}				
	} while (1);	
	printf("\n");
	return 0;
}
