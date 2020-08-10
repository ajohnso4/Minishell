/*******************************************************************************
 * Name        : minishell.c
 * Author      : Andrew Johnson
 * Date        : 3/8/2020
 * Description : minishell
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
*******************************************************************************/
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/*#include <util.h>*/
#include "minishell.h"

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"
#define BUFSIZE 128

sigjmp_buf jmpbuf;

void catch_signal(int sig) {
	if(write(STDOUT_FILENO, "\n", 1) == -1){
		fprintf(stderr, "Error: write() failed. %s", strerror(errno));
	}
	siglongjmp(jmpbuf, 1);
}


int main(int argc, char* argv[]) {

	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = catch_signal;
	action.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &action, NULL) == -1) {
		perror("sigaction");
		return EXIT_FAILURE;
	}

	char path[PATH_MAX];
	char* args[BUFSIZE];
	char* buffer = malloc(sizeof(char) * BUFSIZE  + 1);
	if(buffer == NULL){
		fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}
	int bytes_read, size;
	sigsetjmp(jmpbuf, 1);
	do {
		getcwd(path, sizeof(path));
		if(path == NULL){
			fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("%s[%s]%s$ ", BRIGHTBLUE, path, DEFAULT);
		fflush(stdout);
		bytes_read = read(STDIN_FILENO, buffer, BUFSIZE);
		if(bytes_read == -1){
			fprintf(stderr, "Error: read() failed. %s", strerror(errno));
			continue;
		}
		buffer[bytes_read] = '\0';
		if ((size = parse_args(buffer, args)) == 0) {
			siglongjmp(jmpbuf, 1);
		}
		if (strncmp(args[0], "exit", 4) == 0) {
			free(buffer);
			return EXIT_SUCCESS;
		} else if (strncmp (args[0], "cd", 2) == 0) {
			if (size > 2) {
				fprintf(stderr, "Error: Too many arguments to cd.\n");
			} else {
				cd_command(path, args, size);
			}	
		} else {
			execute_command(args);
		}
		memset(buffer, 0, strlen(buffer));
		memset(args, '\0', sizeof(char*) * size);
	} while (true);
}