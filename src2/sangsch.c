#include "sangsch.h"

/* Functions for printing to shell */
void printWelcomeScreen();
void printShellPrompt();
void appendEnv();


int main(int argc, char *argv[])
{
    // Initalize variables here
    #ifndef DEBUG
    printWelcomeScreen();
    printShellPrompt();
    #endif

    #ifdef DEBUG
    appendEnv();
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
    printf("\n\n");
}

void printShellPrompt()
{
    char buff[MAX_SIZE];
    if (getcwd(buff, sizeof(buff)) == NULL)
        perror("Error getting the current directory: ");
    // user is previously defined in header
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
