#ifndef MINISHELL_H_
#define MINISHELL_H_

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"
#define BUFSIZE 128

/* Function prototypes */
int parse_args(char* buffer, char** args);
void cd_command(char* path, char* argv[], int size);
int execute_command(char* argv[]);

#endif
