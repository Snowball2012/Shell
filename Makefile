all:
	gcc -Wall -g shell.c execution.c parser.c cd.c redirection.c convayor.c -o  chell
	rm -f *~

	

