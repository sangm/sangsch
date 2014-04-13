#include "auxf.h"

void printWelcomeScreen()
{
    int i;

    printf("\n-------------------------------------------------\n");
    printf(ANSI_COLOR_GREEN "\tsangsch designed and implemented by: \n" ANSI_COLOR_RESET);
    for (i = 0; people[i] != NULL; ++i)
        printf(ANSI_COLOR_BLUE "\t\t%s\n", people[i]);
    printf(ANSI_COLOR_RESET "-------------------------------------------------\n");
    print("\n\n");
    
}

void printShellPrompt()
{
    user = getpwuid(geteuid());
    printf(ANSI_COLOR_CYAN "%s $ " ANSI_COLOR_RESET , user->pw_name);
}
