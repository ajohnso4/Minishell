#/*******************************************************************************
# * Name        : makefile
# * Author      : Ali Kolenovic
# * Date        : 3/8/2020
# * Description : compiles spfind
# * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
#/*******************************************************************************
minishell: minishell.o minishell_lib.o
	gcc minishell.o minishell_lib.o -o minishell
minishell.o: minishell.c
	gcc -Wall -Werror -pedantic-errors -c minishell.c
minishell_lib.o: minishell_lib.c minishell.h
	gcc -Wall -Werror -pedantic-errors -c minishell_lib.c
clean:
	rm -f *.o minishell minishell.exe
