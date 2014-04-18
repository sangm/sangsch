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


char *redirectAppend = NULL;

struct redirectStruct {
    char *file;
    short oFlags;
} redirect;


void getUserInput();
void populateGetArgs();
void destroyBuffer();
int checkForString(char *args[], int argCount, char *target);

void printShell()
{
    int i;
    for (i = 0; shellArgv[i] != NULL; ++i)
        printf("%s ", shellArgv[i]);
    printf("\n");
}

int main(int argc, char *argv[], char *envp[])
{
    printWelcomeScreen();
    while(1) {
        printShellPrompt();
        userInput = getchar();
        int status;
        switch(userInput) {
            case '\n':
                break;
            default:
                
                // Returns argv and argc as shellArgv, shellArgc
                getUserInput();

                // Check for >>, <, >, |
                if (checkForString(shellArgv, shellArgc, ">>") != 0)
                    break;
                // Order here matters
                
                checkForString(shellArgv, shellArgc, "<");
                checkForString(shellArgv, shellArgc, ">>");
                checkForString(shellArgv, shellArgc, ">");
                checkForString(shellArgv, shellArgc, "|");
                printShell();


                pid_t pid;
                if ((pid = fork()) < 0) perror("Fork failed: ");
                else if (pid > 0) {
                    // parent block. 
                    wait(&status);
                }
                else if (pid == 0) {
                    int fd = open(redirectAppend, O_WRONLY | O_CREAT | O_APPEND, 0600);
                    dup2(fd, 1);
                    close(fd);
                    execvp(shellArgv[0], shellArgv);
                }
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
                fprintf(stdout, "Unexpected newline\n");
                return -1;
            }
            else if (strcmp(args[i+1], target) == 0) {
                fprintf(stdout, "Syntax error near %s \n", args[i]);
                return -1;
            }
            else {
                // Next argument is valid
                if (strcmp(target, ">>") == 0) {
                    int strlength = strlen(args[i+1])+1;
                    redirectAppend = (char *)malloc(strlength);
                    strcpy(redirectAppend, args[i+1]);
                    // Shift all the args down 1
                    // argv = [cat, foo, >>, foo1]
                    // argv = [cat, foo] -> stdout points to foo
                    int j;
                    // Check for 1 off error when I get some sleep :)
                    for (j = i; j < shellArgc-1; ++j) {
                        args[j] = args[j+1];
                        args[j] = '\0';
                    }
                    shellArgc -= 2;
                }
                else if (strcmp(target,"<") == 0) {
                    printf("stdin\n");
                    int strlength = strlen(args[i+1])+1;
                    redirect.file = (char *)malloc(strlength);
                    redirect.oFlags = O_RDONLY;
                    strcpy(redirect.file, args[i+1]);
                    int j;
                    for (j = i; j < shellArgc-1; ++j) {
                        args[j] = args[j+1];
                        args[j] = '\0';
                    }
                    shellArgc -= 2;
                }
                else if (strcmp(target, ">") == 0) {
                    printf("stdout redirected, truncate");
                }

            }
        }
    }

    return 0;
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
    bzero(shellArgv, 5);
    if (redirect.file) redirect.file = "\0";
    redirect.oFlags = 0;
    shellArgc = bufferCount = 0;
}

