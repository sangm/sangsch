#ifndef __AUXF_H
#define __AUXF_H

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static const char *people[] = {
    "Sang Mercado",
    "Jared Pruett",
    "Mason Egger",
    "Derrick Green",
    NULL
};

struct passwd *user;

void printWelcomeScreen();
void printShellPrompt();

#endif
