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
        int status, fdin = 0, fdout = 1;
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

                // Check for >>, <, >, |

                while ((checkForString(shellArgv, shellArgc, ">>") == 0) 
                  || checkForString(shellArgv, shellArgc, ">") == 0)
                    fdout = open(redirect.file, redirect.oFlags, 0600);
                while ((checkForString(shellArgv, shellArgc, "<") == 0))
                    fdin = open(redirect.file, redirect.oFlags);
                
                pid_t pid;
                pid = fork();
                switch(pid) {
                    case -1:
                        perror("Fork failed due to: ");
                        exit(1);
                        break;
                    case 0:
                        if (redirect.file) {
                            if (redirect.output == 1) {
                                dup2(fdout, 1);
                                close(fdout);
                            }
                            if (redirect.input == 1) {
                                dup2(fdin, 0);
                                close(fdin);
                            }
                        }
                        execvp(shellArgv[0], shellArgv);
                        break;
                    default:
                        wait(&status);
                        if (redirect.output == 1) close(fdout);
                        if (redirect.input == 1) close(fdin);
                        break;
                }

                printf("after command\n");

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
                    int j;
                    strlength = strlen(args[0]) + 1;
                    pipeS.inPipe= (char *)malloc(strlength);
                    strcpy(pipeS.inPipe, args[0]);
                    for (j = 0; j < i; ++j)
                        pipeS.inPipeArgs[j] = args[j];


                    //strlength = strlen(args[i+1]) + 1;
                    //redirect.OutCommand = (char *)malloc(strlength);
                    //strcpy(redirect.OutCommand, args[i+1]);
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
    int i;
    while(shellArgv[i]) shellArgv[i] = NULL;
    if (redirect.file) redirect.file = "\0";
    redirect.input = redirect.output = redirect.oFlags = 0;
    pipeS.pipe[0] = pipeS.pipe[1] = -1;
    shellArgc = bufferCount = 0;
}

