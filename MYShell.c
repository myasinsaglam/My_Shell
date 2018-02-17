#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> //waitpid()
#include <unistd.h> //chdir,fork,exec,pid_t
#include <string.h>
#include <termios.h>

#define BUFSIZE 1024
#define MAXLINE 100
#define TOKENSIZE 50
#define TOKEN_DELIMETERS " \t\r\n\a"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KWHT  "\x1B[37m"
#define BOLD  "\x1b[1m"

char **tokenize_line(char *line);
int MYSh_launch(char **args);
int execute_cmd(char **args);


int main(int argc,char **argv ){
    printf("My name is M.Yasin SAGLAM... Welcome to MYShell...\n");
    char **args,**lines,*shown_text;
    int status,i=0;
    char username[100];
    char cwd[1024];
    ssize_t bufsize;
    lines=(char**)calloc(MAXLINE,sizeof(char*));
    if(!lines){
        printf("Shell allocation error occured!!! Quitting...");
        exit(0);
    }
    shown_text=(char*)malloc(sizeof(char)*BUFSIZE*2);
    if (!shown_text){
        printf("Shell allocation error occured!!! Quitting...");
        exit(0);
    }
    strcpy(shown_text,"MYShell> ");
    do{
	printf("%s", BOLD);
	getcwd(cwd, sizeof(cwd));
	strcpy(username,getenv("USER"));
	printf("%s%s@ubuntu",KYEL,username);
	printf("%s:", KWHT);
	printf("%s%s",KBLU,cwd);
	printf("%s$", KWHT);
        bufsize = 0;
        getline(&lines[i], &bufsize, stdin);
        //printf("%s",lines[i]);
        args=tokenize_line(lines[i]);
        status=execute_cmd(args);
        free(args);
        i++;
    }while (status);
    
    return EXIT_SUCCESS;
}

char **tokenize_line(char *line) {
    int bufsize = TOKENSIZE, index= 0;
    char *token;
    char **tokens = malloc(bufsize * sizeof(char*));
    if (!tokens) {
        printf("MYSh: allocation error in tokenize operations!!! Quitting...\n");
        exit(0);
    }

    token = strtok(line, TOKEN_DELIMETERS);
    while (token != NULL) {
        tokens[index] = token;
        index++;

        if (index >= bufsize) {
            bufsize += TOKENSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                printf("MYSh: allocation error in tokenize operations!!! Quitting...\n");
                exit(0);
            }
        }

        token = strtok(NULL, TOKEN_DELIMETERS);
    }
    tokens[index] = NULL;
    return tokens;
}

int execute_cmd(char **args) {

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    else if(strcmp(args[0],"exit") == 0) {
        printf("\nSee you again. Bye...\n");
        exit(0);
    }
    else if(strcmp(args[0],"cd") == 0){
        if (args[1] == NULL) {
            printf("MYSh: Error!!! expected argument to \"cd\"\n");
        }
        else if (chdir(args[1]) != 0) { //changes the current working directory to that specified in path.On success, zero is returned
            printf("\nDirectory not changed...\n");
        }
        return 1;
    }
    return MYSh_launch(args);
}

int MYSh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // if child process is running
        if (execvp(args[0], args) == -1) {
            perror("MYShChildRunError");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Child process not created fork error...
        perror("MYShChildForkError");
    } else {
        // if Parent process is running
        do {
            wpid = waitpid(pid, &status, WUNTRACED); //wait for child process
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}




