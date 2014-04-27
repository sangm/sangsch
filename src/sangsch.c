#include "sangsch.h"

/* Functions for printing to shell */
void printWelcomeScreen();
void printShellPrompt();
void printShell(char *arg[]);

void getUserInput();
void populateGetArgs();
void destroyBuffer();
void appendEnv();
char *checkForDollar(char *argv[], int argc);
int processArgs(char *args[], int argCount, char *target);
int dIndex;

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
    appendEnv();
    printWelcomeScreen();
    while(1) {
        printShellPrompt();
        userInput = getchar();

        int status, fdin, fdout, pipeStatus; 
        Redirect.file = NULL;
        Pipe.inPipe = NULL;
        Pipe.outPipe = NULL;
        fdin = pipeStatus = 0;
        fdout = 1;
        switch(userInput) {
            case '\n':
                break;
            default:
                getUserInput();
                // Returns argv and argc as shellArgv, shellArgc

                // Check for >>, <, >, |
                while ((processArgs(shellArgv, shellArgc, ">>") == 0) || 
                        processArgs(shellArgv, shellArgc, ">")  == 0) {
                    fdout = open(Redirect.file, Redirect.oFlags, 0600);
                }
                while ((processArgs(shellArgv, shellArgc, "<") == 0)) {
                    fdin = open(Redirect.file, Redirect.oFlags);
                }

                if (processArgs(shellArgv, shellArgc, "|") == 0) {
                    if (pipe(Pipe.pipe) < 0) perror("Pipe failed: ");
                    pipeStatus = 1;
                }

                char *dCommand = checkForDollar(shellArgv, shellArgc);
                if (dCommand != NULL) {
                    // Execute the command
                    pid_t pid;
                    char buffer[256];
                    char *arg;
                    int fd;
                    int status;
                    int n;
                    fd = open("tempFile", O_RDWR | O_CREAT, 0666);
                    pid = fork();
                    switch(pid) {
                        case -1: 
                            printf("Fork failed\n");
                            exit(1);
                            break;
                        case 0:
                            dup2(fd, 1);
                            n = execlp(dCommand, dCommand, NULL);
                            if (n == -1) {printf("Not executed"); exit(1);}
                        default:
                            wait(&status);
                            free(dCommand);
                            n = open("tempFile", O_RDONLY);
                            read(n, buffer, 256);
                            shellArgc--;
                            shellArgv[dIndex] = '\0';
                            arg = strtok(buffer, "\n");
                            while (arg != NULL) {
                                int stat;
                                pid_t pid;
                                pid = fork();
                                shellArgv[dIndex] = arg;
                                dIndex++;
                                shellArgc++;

                                if (pid == 0)
                                    execlp(shellArgv[0], shellArgv[0], arg, NULL);
                                else if (pid > 0) 
                                    wait(&stat);
                                arg = strtok(NULL, "\n");
                            }
                            remove("tempFile");
                            break;
                    }

                }

                pid_t pid, pid2;
                pid = fork();
                if (pid < 0) {
                    perror("Fork failed due to: ");
                    exit(1);
                } 
                else if (pid == 0) {
                    if (Redirect.file) 
                        if (Redirect.output == 1) {
                            dup2(fdout, 1);
                            close(fdout);
                        }
                        if (Redirect.input == 1) {
                            dup2(fdin, 0);
                            close(fdin);
                        }
                    if (pipeStatus == 1) {
                        dup2(Pipe.pipe[1], 1);
                        close(Pipe.pipe[1]);
                        close(Pipe.pipe[0]);
                        execvp(Pipe.inPipe, Pipe.inPipeArgs);
                        // if execvp returns negative
                        printf("Not valid command\n");
                        exit(1);
                    }
                    execvp(shellArgv[0], shellArgv);
                    printf("%s: not valid command\n", shellArgv[0]);
                }
                if (pipeStatus == 1) {
                    pid2 = fork();
                    if (pid2 == 0) {
                        dup2(Pipe.pipe[0], 0);
                        close(Pipe.pipe[0]);
                        close(Pipe.pipe[1]);
                        execvp(Pipe.outPipe, Pipe.outPipeArgs);
                        printf("Not valid command\n");
                        exit(1);
                    }
                    close(Pipe.pipe[0]);
                    close(Pipe.pipe[1]);
                }
                if (pipeStatus == 1)
                    while(pid + pid2 > 0) {
                        int p = wait(NULL);
                        if (p == pid) pid = 0;
                        if (p == pid2) pid2 = 0;
                    }
                else
                    wait(&status);
                if (Redirect.output == 1) close(fdout);
                if (Redirect.input == 1) close(fdin);
                break;
        }
    }
}

int processArgs(char *args[], int argCount, char *target) 
{
    int i;
    int result;

    for (i = 0; i < argCount; ++i) {
        if ((result = strcmp(args[i], target)) == 0) {
            if (args[i+1] == NULL || (strcmp(args[i+1], target) == 0)) { 
                fprintf(stdout, "Syntax error near %s \n", args[i]);
                return -1;
            }
            else {
                /* Next argument is valid */
                if (strcmp(target, ">>") == 0) {
                    Redirect.oFlags = O_WRONLY | O_CREAT | O_APPEND;
                    Redirect.output = 1;
                }
                else if (strcmp(target, ">") == 0) {
                    Redirect.oFlags = O_WRONLY | O_CREAT | O_TRUNC;
                    Redirect.output = 1;
                }
                else if (strcmp(target,"<") == 0) {
                    Redirect.oFlags = O_RDONLY;
                    Redirect.input = 1;
                }
                else if (strcmp(target, "|") == 0) {
                    int strlength;
                    int j, k = 0;
                    strlength = strlen(args[0]) + 1;
                    Pipe.inPipe = (char *)malloc(strlength);
                    strcpy(Pipe.inPipe, args[0]);
                    for (j = 0; j < i; ++j)
                        Pipe.inPipeArgs[j] = args[j];
                    Pipe.inPipeArgs[j] = NULL;

                    strlength = strlen(args[i+1]) + 1;
                    Pipe.outPipe = (char *)malloc(strlength);
                    strcpy(Pipe.outPipe, args[i+1]);
                    for (j = i + 1; j < argCount; ++j)
                        Pipe.outPipeArgs[k++] = args[j];
                    Pipe.outPipeArgs[k] = NULL;
                }
                if (Redirect.output == 1 || Redirect.input == 1) {
                    int strlength = strlen(args[i+1])+1;
                    Redirect.file = (char *)malloc(strlength);
                    strcpy(Redirect.file, args[i+1]);
                }
                /* This won't work for second half. Have to implement 
                 * If the second half is longer than 1, eliminate the rest
                 * For piping :)
                 **/
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

char *checkForDollar(char *argv[], int argc)
{
    // Argv should be processed at this point
    char *arg;
    int i = 0;
    while(argv[i] != NULL) {
        if ((arg = strpbrk(argv[i], "$")) != NULL)
            break;
        ++i;
    }
    if (i != argc) {// means found
        int len = strlen(argv[i]);
        if (argv[i][0] != '$') return NULL;
        if (argv[i][1] != '\0' && argv[i][1] != '(') return NULL;
        if (argv[i][len-1] != '\0' && argv[i][len-1] != ')') return NULL;
        char *c;
        c = strchr(argv[i], '(');
        char *t = (char *)malloc(sizeof(argv[i]));;
        strncpy(t, ++c, len-3);
        dIndex = i;
        return t;
    }
    return NULL;
}

void getUserInput()
{
    destroyBuffer();
    while ((userInput != '\n') && (bufferCount < MAX_SIZE)) {
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
void emptyArgs(char *argv[])
{
    int i;
    i = 0;
    while(argv[i])
        argv[i++] = '\0';
}
void destroyBuffer()
{
    emptyArgs(shellArgv);
    emptyArgs(Pipe.inPipeArgs);
    emptyArgs(Pipe.outPipeArgs);

    if (Redirect.file) { Redirect.file = "\0";}
    Redirect.input = Redirect.output = Redirect.oFlags = 0;
    Pipe.pipe[0] = Pipe.pipe[1] = -1;
    shellArgc = bufferCount = dIndex = 0;
    if (Pipe.inPipe != NULL) {free(Pipe.inPipe); Pipe.inPipe = NULL;}
    if (Pipe.outPipe != NULL) {free(Pipe.outPipe); Pipe.outPipe = NULL;}
    if (Redirect.file != NULL) {free(Redirect.file); Redirect.file = NULL;}
}

void printWelcomeScreen()
{
    int i;
    printf("\n-------------------------------------------------\n");
    printf(ANSI_COLOR_GREEN "\tsangsch designed and implemented by: \n" ANSI_COLOR_RESET);
    for (i = 0; people[i] != NULL; ++i)
        printf(ANSI_COLOR_BLUE "\t\t%s\n", people[i]);
    printf(ANSI_COLOR_RESET "-------------------------------------------------\n");
    printf("\n\n");
}

void printShellPrompt()
{
    struct passwd *user;
    char buff[MAX_SIZE];
    if (getcwd(buff, sizeof(buff)) == NULL)
        perror("Error getting the current directory: ");
    user = getpwuid(geteuid());
    printf(ANSI_COLOR_CYAN "%s:%s $ " ANSI_COLOR_RESET, user->pw_name, buff);
}

void appendEnv()
{
        /* This function should dynamically allocate length of strings
         * dynamically, but for the scope of this project, 256 seems fine
         **/
        char localEnv[256];
        char *localEnvp;
        char buff[MAX_SIZE];
        char *temp;
        /* This part violates DRY principle, but... C :p */
        if (getcwd(buff, sizeof(buff)) == NULL)
            perror("Error getting the current directory: ");

        temp = strrchr(buff, '/');
        while(*temp) *temp++ = '\0';

        localEnvp = getenv("PATH");
        strcpy(localEnv, localEnvp);
        strcat(localEnv, ":");
        strcat(buff, "/bin");
        strcat(localEnv, buff);
        setenv("PATH", localEnv, 1);
}
