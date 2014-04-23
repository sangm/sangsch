#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_MAX 2048
char userInput = '\0';
char buffer[BUFFER_MAX];
char bufferCount = 0;
char *shellArgv[10];
int shellArgc = 0;

struct redirectStruct {
    char *file;
    short oFlags;
    short input;
    short output;
} redirect;

struct pipeStruct {
    int pipe[2];
    char *inPipe;
    char *outPipe;
    char *inPipeArgs[5];
    char *outPipeArgs[5];
} pipeS;

void getUserInput();
void populateGetArgs();
void destroyBuffer();
int checkForString(char *args[], int argCount, char *target);

void printShell(char *arg[])
{
    int i;
    printf("==============\n");
    for (i = 0; arg[i] != NULL; ++i)
        printf("%s ", arg[i]);
    printf("\n============\n\n");
}

int main(int argc, char *argv[])
{
    printWelcomeScreen();
    while(1) {
        printShellPrompt();
        userInput = getchar();
        int status, fdin = 0, fdout = 1, pipeStatus = 0;
        switch(userInput) {
            case '\n':
                break;
            default:
                getUserInput();
                // Returns argv and argc as shellArgv, shellArgc
                #ifdef DEBUG
                shellArgv[0] = "ls";
                shellArgv[1] = "|";
                shellArgv[2] = "cat";
                shellArgc = 3;
                #endif

                char *environment = getenv("PATH");
                strcat(environment, ";/Users/sang/Desktop/C++/sangsch/src/bin");
                putenv(environment);
                environment = getenv("PATH");
                printf("%s\n", environment);
                exit(1);

                // Check for >>, <, >, |
                while ((checkForString(shellArgv, shellArgc, ">>") == 0) 
                  || checkForString(shellArgv, shellArgc, ">") == 0)
                    fdout = open(redirect.file, redirect.oFlags, 0600);
                while ((checkForString(shellArgv, shellArgc, "<") == 0))
                    fdin = open(redirect.file, redirect.oFlags);

                if (checkForString(shellArgv, shellArgc, "|") == 0) {
                    if (pipe(pipeS.pipe) < 0) perror("Pipe failed: ");
                    pipeStatus = 1;
                }
                pid_t pid, pid2;
                pid = fork();
                if (pid < 0) {
                    perror("Fork failed due to: ");
                    exit(1);
                } 
                else if (pid == 0) {
                    if (redirect.file) 
                        if (redirect.output == 1) {
                            dup2(fdout, 1);
                            close(fdout);
                        }
                        if (redirect.input == 1) {
                            dup2(fdin, 0);
                            close(fdin);
                        }
                    if (pipeStatus == 1) {
                        dup2(pipeS.pipe[1], 1);
                        close(pipeS.pipe[1]);
                        close(pipeS.pipe[0]);
                        execvp(pipeS.inPipe, pipeS.inPipeArgs);
                        // if execvp returns negative
                        printf("Not valid command\n");
                        exit(1);
                    }
                    execvp(shellArgv[0], shellArgv);
                    printf("Not valid command\n");
                }
                if (pipeStatus == 1) {
                    pid2 = fork();
                    if (pid2 == 0) {
                        dup2(pipeS.pipe[0], 0);
                        close(pipeS.pipe[0]);
                        close(pipeS.pipe[1]);
                        execvp(pipeS.outPipe, pipeS.outPipeArgs);
                        printf("Not valid command\n");
                        exit(1);
                    }
                    close(pipeS.pipe[0]);
                    close(pipeS.pipe[1]);
                }
                if (pipeStatus == 1)
                    while(pid + pid2 > 0) {
                        int p = wait(NULL);
                        if (p == pid) pid = 0;
                        if (p == pid2) pid2 = 0;
                    }
                else
                    wait(&status);
                if (redirect.output == 1) close(fdout);
                if (redirect.input == 1) close(fdin);
                break;
        }
    }
}

int checkForString(char *args[], int argCount, char *target) 
{
    int i;
    int result;

    for (i = 0; i < argCount; ++i) {
        if ((result = strcmp(args[i], target)) == 0) {
            if (args[i+1] == NULL) { 
                return -1;
            }
            else if (strcmp(args[i+1], target) == 0) {
                fprintf(stdout, "Syntax error near %s \n", args[i]);
                return -1;
            }
            else {
                // Next argument is valid
                if (strcmp(target, ">>") == 0) {
                    redirect.oFlags = O_WRONLY | O_CREAT | O_APPEND;
                    redirect.output = 1;
                }
                else if (strcmp(target, ">") == 0) {
                    redirect.oFlags = O_WRONLY | O_CREAT | O_TRUNC;
                    redirect.output = 1;
                }
                else if (strcmp(target,"<") == 0) {
                    redirect.oFlags = O_RDONLY;
                    redirect.input = 1;
                }
                else if (strcmp(target, "|") == 0) {
                    int strlength;
                    int j, k = 0;
                    strlength = strlen(args[0]) + 1;
                    pipeS.inPipe = (char *)malloc(strlength);
                    strcpy(pipeS.inPipe, args[0]);
                    for (j = 0; j < i; ++j)
                        pipeS.inPipeArgs[j] = args[j];
                    pipeS.inPipeArgs[j] = NULL;

                    strlength = strlen(args[i+1]) + 1;
                    pipeS.outPipe = (char *)malloc(strlength);
                    strcpy(pipeS.outPipe, args[i+1]);
                    for (j = i + 1; j < argCount; ++j)
                        pipeS.outPipeArgs[k++] = args[j];
                    pipeS.outPipeArgs[k] = NULL;
                }
                // Shift all the args down 1
                // argv = [cat, foo, >>, foo1]
                // argv = [cat, foo] -> stdout points to foo
                // Check for 1 off error when I get some sleep :)
                if (redirect.output == 1 || redirect.input == 1) {
                    int strlength = strlen(args[i+1])+1;
                    redirect.file = (char *)malloc(strlength);
                    strcpy(redirect.file, args[i+1]);
                }
                // This won't work for second half. Have to implement 
                // If the second half is longer than 1, eliminate the rest
                // For piping :)
                int j;
                for (j = i; j < shellArgc-1; ++j) 
                    args[j] = args[j+2];
                shellArgc -= 2;
                return 0;
            }
        }
    }
    return -1;
}
void getUserInput()
{
    destroyBuffer();
    while ((userInput != '\n') && (bufferCount < BUFFER_MAX)) {
        buffer[bufferCount++] = userInput;
        userInput = getchar();
    }
    buffer[bufferCount] = '\0';
    populateGetArgs();
}
void populateGetArgs()
{
    char *argPointer;
    argPointer = strtok(buffer, " ");
    while (argPointer != NULL) {
        shellArgv[shellArgc++] = argPointer;
        argPointer = strtok(NULL, " ");
    }
    shellArgv[shellArgc] = NULL;
}
void destroyBuffer()
{
    int i = 0;
    while(shellArgv[i]) shellArgv[i++] = NULL;
    i = 0;
    while(pipeS.inPipeArgs[i] || pipeS.outPipeArgs[i]) {
        pipeS.inPipeArgs[i] = pipeS.outPipeArgs[i] = NULL;
        ++i;
    }
    if (redirect.file) { free(redirect.file); redirect.file = "\0";}
    redirect.input = redirect.output = redirect.oFlags = 0;
    pipeS.pipe[0] = pipeS.pipe[1] = -1;
    shellArgc = bufferCount = 0;
}
