#include "auxf.h"

void printWelcomeScreen()
{
    int i;

    printf("\n-------------------------------------------------\n");
    printf(ANSI_COLOR_GREEN "\tsangsch designed by: \n" ANSI_COLOR_RESET);
    for (i = 0; people[i] != NULL; ++i)
        printf(ANSI_COLOR_BLUE "\t\t%s\n", people[i]);
    printf("-------------------------------------------------\n");
    
}

void printShellPrompt()
{

}
