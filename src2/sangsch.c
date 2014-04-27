#include "sangsch.h"

/* Functions for printing to shell */
void printWelcomeScreen();
void printShellPrompt();
void printShell();
void populateGetArgs(char *buff, char *args[], int *argc);
void getUserInput();
void emptyArgs();
void appendEnv();
void destroyBuffer();
int processArgs(char *args[], int argc, char *target);

int main(int argc, char *argv[])
{
    /* Initalize variables here */
    int fdin, fdout, pipeStatus, status;
    Pipe.inPipe = Pipe.outPipe = NULL;    
    fdin = pipeStatus = 0;
    fdout = 1;
    user = getpwuid(geteuid());
    printWelcomeScreen();
    appendEnv();
    #ifndef DEBUG
    while(1) {
        printShellPrompt();
        userInput = getchar();
        switch(userInput) {
            case '\n':
                break;
            default:
                /* Returns valid shellArgv, shellArgc */
                getUserInput();
                
                while ((processArgs(shellArgv, shellArgc, ">>") == 0) || 
                        processArgs(shellArgv, shellArgc, ">")  == 0) {
                    fdout = open(Redirect.file, Redirect.oFlags, 0600);
                    if (Redirect.file != NULL) {
                        free(Redirect.file);
                        Redirect.file = NULL;
                    }
                }

                while ((processArgs(shellArgv, shellArgc, "<") == 0)) {
                    fdin = open(Redirect.file, Redirect.oFlags);
                    if (Redirect.file != NULL) {
                        free(Redirect.file);
                        Redirect.file = NULL;
                    }
                }

                if (processArgs(shellArgv, shellArgc, "|") == 0) {
                    if (pipe(Pipe.pipe) < 0)
                        perror("Pipe failed due to: ");
                    pipeStatus = 1;
                }

                /* Executing the commands */
                pid_t pid, pid2;
                pid = fork();
                
                if (pid < 0) {
                    perror("Fork failed due to: ");
                    exit(1);
                }
                else if (pid == 0) {
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
                        /* if execvp fails */
                        printf("%s not valid command\n", Pipe.inPipe);
                        exit(1);
                    }
                    execvp(shellArgv[0], shellArgv);
                    printf("%s: not valid command\n", shellArgv[0]);
                }
                if (pipeStatus == 1) {
                    pid2 = fork();
                    switch(pid2) {
                        case -1:
                            perror("pid2 fork failed due to: ");
                            exit(1);
                        case 0:
                            dup2(Pipe.inPipe[0], 0);
                            close(Pipe.pipe[0]);
                                close(Pipe.pipe[1]);
                                execvp(Pipe.outPipe, Pipe.outPipeArgs);
                                printf("%s not valid command\n", Pipe.outPipe);
                                exit(1);
                            default:
                                close(Pipe.pipe[0]);
                                close(Pipe.pipe[1]);
                        }
                    }
                if (pipeStatus == 1)
                    while(pid + pid2 > 0) {
                        int p = wait(NULL);
                        if (p == pid) pid = 0;
                        if (p == pid2) pid = 0;
                    }
                else
                    wait(&status);
                if (Redirect.output == 1) close(fdout);
                if (Redirect.input == 1) close(fdin);
                break;
            }
    }
    #endif

    return 0;
}

void printWelcomeScreen()
{
    int i;
    printf("\n---------------------------------------------\n");
    printf(ANSI_COLOR_GREEN "\tsangsch designed and implemented by: \n" ANSI_COLOR_RESET);
    while(people[i] != NULL)
        printf(ANSI_COLOR_BLUE "\t\t%s\n", people[i++]);
    printf(ANSI_COLOR_RESET "\n\n");
}

void printShellPrompt()
{
    char buff[MAX_SIZE];
    if (getcwd(buff, sizeof(buff)) == NULL)
        perror("Error getting the current directory: ");
    /* user is previously defined in header */
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
void getUserInput()
{
    destroyBuffer();
    while ((userInput != '\n') && (bufferCount < MAX_SIZE)) {
        buffer[bufferCount++] = userInput;
        userInput = getchar();
    }
    buffer[bufferCount] = '\0';
    populateGetArgs(buffer, shellArgv, &shellArgc);
}

void populateGetArgs(char *buff, char *args[], int *argc)
{
    char *argPointer;
    argPointer = strtok(buffer, " ");
    while (argPointer != NULL) {
        args[(*argc)++] = argPointer;
        argPointer = strtok(NULL, " ");
    }
    args[*argc] = NULL;
}

void destroyBuffer() 
{
    emptyArgs(shellArgv);
    emptyArgs(Pipe.inPipeArgs);
    emptyArgs(Pipe.outPipeArgs);

    if (Redirect.file)
        Redirect.file = '\0';
    if (Pipe.inPipe != NULL) {
        free(Pipe.inPipe); 
        Pipe.inPipe = NULL;
    }
    if (Pipe.outPipe != NULL) {
        free(Pipe.outPipe);
        Pipe.outPipe = NULL;
    }

    Redirect.input = Redirect.output = Redirect.oFlags = 0;
    Pipe.pipe[0] = Pipe.pipe[1] = -1;
    shellArgc = bufferCount = 0;
}

void emptyArgs(char *argv[])
{
    int i;
    i = 0;
    while(argv[i]) 
        argv[i++] = '\0';
}

void printShell(char *argv[])
{
    int i;
    i = 0;
    printf("\n=========================\n");
    while (argv[i] != NULL) printf("%s ", argv[i++]);
    printf("\n=========================\n");
}

int processArgs(char *args[], int argc, char *target)
{
    int i;
    int result;
    for (i = 0; i < argc; ++i) {
        if ((result = strcmp(args[i], target)) == 0) {
            if (args[i+1] == NULL || strcmp(args[i+1], target) == 0) {
                fprintf(stderr, "Syntax error near %s \n", args[i]);
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
                else if (strcmp(target, "<") == 0) {
                    Redirect.oFlags = O_RDONLY;
                    Redirect.input = 1;
                }
                else if (strcmp(target, "|") == 0) {
                    int len;
                    int j, k;
                    j = k = 0;
                    len = strlen(args[0]) + 1; /* +1 for null char */
                    Pipe.inPipe = (char *)malloc(len);
                    strcpy(Pipe.inPipe, args[0]);

                    /* Splitting the args in half for piping.
                     * inPipe = first half, outPipe = right half
                     **/
                    for (j = 0; j < i; ++j)
                        Pipe.inPipeArgs[j] = args[j];
                    Pipe.inPipeArgs[j] = NULL;
                    
                    len = strlen(args[i+1]) + 1;
                    Pipe.outPipe = (char *)malloc(len);
                    strcpy(Pipe.outPipe, args[i+1]);
                    for (j = i+1; j < argc; ++j)
                        Pipe.outPipeArgs[k++] = args[j];
                    Pipe.outPipeArgs[k] = NULL;
                }

                if (Redirect.output == 1 || Redirect.input == 1) {
                    int len;
                    len = strlen(args[i+1]) + 1;
                    Redirect.file = (char *)malloc(len);
                    strcpy(Redirect.file, args[i+1]);
                }

                /* This won't work for second half if pipe is present */
                int j;
                for (j = i; j < shellArgc - 1; ++j)
                        args[j] = args[j+2];
                shellArgc -= 2;
                return 0;
            }
        }
    }
    return -1;
}





