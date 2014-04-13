#ifndef __AUXF_H
#define __AUXF_H

#define MAX_CHAR_LENGTH 1024
#define MAX_SIZE 2048
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

static const char *people[] = {
    "Sang Mercado",
    "Jared Pruett",
    "Mason Egger",
    "Derrick Green",
    NULL
};

char commandBuffer[MAX_SIZE];
static char userInput = '\0';
static unsigned charCount = 0;

struct passwd *user;

void printWelcomeScreen();
void printShellPrompt();
void getTextFromShell();

#endif
