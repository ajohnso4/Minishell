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
#include "minishell.h"




int parse_args(char* buffer, char** args) {
	char *pch;
	int i = 0;
	pch = strtok(buffer, " \n");
	while (pch != NULL) {
		args[i++] = strdup(pch);
		pch = strtok(NULL, " \n");
	}
	return i;
}

void cd_command(char* path, char* argv[], int size) {
	if (size == 1 || strncmp(argv[1], "~", 1) == 0) {
		char *lgn;
		struct passwd *wd;
		lgn = getlogin();
		wd = getpwnam(lgn);
		if(wd == NULL){
			fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
			return;
		}
		chdir(wd->pw_dir);
		strcpy(path, wd->pw_dir);
	} else {
		char full_path[PATH_MAX];
		realpath(argv[1], full_path);
		if (chdir(full_path) == -1) {
			fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", full_path, strerror(errno));
	 		return;
		}
	}
}


int execute_command(char* argv[]) {
	pid_t pid;
	
	if ((pid = fork()) < 0) {
        fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        // We're in the child
        if (execvp(argv[0], argv) == -1) {
            fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
        
	} else {
        // We're in the parent
		int status;
        do {
			pid_t w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
			if (w == -1) {
				fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
				return EXIT_FAILURE;
			}
			if (WIFEXITED(status)) {
				if (WEXITSTATUS(status) == EXIT_FAILURE) {
				 	return EXIT_FAILURE;
				} else {
					return EXIT_SUCCESS;
				}
			}
			return EXIT_FAILURE;
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
        
    }
}