all:
	gcc -Wall -g shell.c execution.c parser.c cd.c redirection.c -o  chell
	rm -f *~

	

