#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


#define clear() printf("\033[H\033[J")
#define IN_BUFSIZE 1024


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};


char *ssh_read_line(void)
{
size_t bufsize = IN_BUFSIZE;
size_t position;
int c;
char *buffer = malloc(sizeof(char) * bufsize);

position = 0;
if(!buffer)
{
fprintf(stderr,"Memory allocation error.\n");
exit(EXIT_FAILURE);
}
while(1)
{
/* read line */
c = getchar();
if (c == EOF || c == '\n')
{
buffer[position] = '\0';
return buffer;
}
else
{
buffer[position] = c;
}
position++;
if (position >= bufsize)
{
bufsize += IN_BUFSIZE;
buffer = realloc(buffer, bufsize);
if (!buffer)
{
fprintf(stderr, "Memory allocation error");
exit(EXIT_FAILURE);
}
}
}

}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **ssh_split_line(char *line)
{
int bufsize = LSH_TOK_BUFSIZE, position = 0;
char **tokens = malloc(bufsize * sizeof(char*));
char *token;
if (!tokens)
{
fprintf(stderr, "lsh: allocation error\n");
	exit(EXIT_FAILURE);
}
token = strtok(line, LSH_TOK_DELIM);
while (token != NULL)
{
tokens[position] = token;
	position++;

if (position >= bufsize)
{
bufsize += LSH_TOK_BUFSIZE;
tokens = realloc(tokens, bufsize * sizeof(char*));
if (!tokens)
{
fprintf(stderr, "ssh: allocation error\n");
exit(EXIT_FAILURE);
}
}

token = strtok(NULL, LSH_TOK_DELIM); 
}
tokens[position] = NULL;
return tokens;
}


int ssh_launch(char **args)
{
pid_t pid, wpid;
int status;

pid = fork();
if (pid == 0)
{
if (execvp(args[0], args) == -1)
{
perror("lsh");
}
exit(EXIT_FAILURE);
}
else if (pid < 0)
{
perror("lsh");
}
else
{
do
{
wpid = waitpid(pid, &status, WUNTRACED);
pid = wpid;
} while (!WIFEXITED(status) && !WIFSIGNALED(status));

}
return (1);

}

int lsh_num_builtins() {
	          return (sizeof(builtin_str) / sizeof(char *));
}       

int ssh_execute(char **args)
{


/*
 *   List of builtin commands, followed by their corresponding functions.
 *    */

	
int i;

if (args[0] == NULL) {

	return 1;
}

for (i = 0; i < lsh_num_builtins(); i++) {

	if (strcmp(args[0], builtin_str[i]) == 0) {
		return (*builtin_func[i])(args);
		}
	}

return ssh_launch(args);
}


void int_clear()
{
clear();
printf("\nSimple Shell...\n");

}
void input_loop()
{
char *line;
char **args;
int status;
do{
printf("\n$ ");
line = ssh_read_line();

args = ssh_split_line(line);
status = ssh_execute(args);

free(line);
free(args);
}while(status);

}
int main(void)
{


input_loop();
return (0);
}

