#include "sangsch.h"

/* Functions for printing to shell */
void printWelcomeScreen();
void printShellPrompt();


int main(int argc, char *argv[])
{
    // Initalize variables here
    
    printWelcomeScreen();
    printShellPrompt();
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
