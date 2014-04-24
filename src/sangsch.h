#ifndef __SANGSCH_H
#define __SANGSCH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>

#define MAX_CHAR_LENGTH 1024
#define BUFFER_MAX 2048
#define MAX_SIZE 2048
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct passwd *user;

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

struct d_struct {
    char *comm;
    char *argv[5];
} dStruct;

const char *people[] = {
    "Sang Mercado",
    "Jared Pruett",
    "Mason Egger",
    "Derrick Green",
    NULL
};

char userInput = '\0';
char buffer[BUFFER_MAX];
int bufferCount = 0;
char *shellArgv[10];
int shellArgc = 0;

#endif
