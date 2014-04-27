#ifndef __SANGSCH_H
#define __SANGSCH_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>

#define MAX_SIZE 2048
#define MAX_ARGS 10

/* Constants for colored output */

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct redirectStruct {
    char *file;
    short oFlags;
    short input;
    short output;
} Redirect;

struct pipeStruct {
    int pipe[2];
    char *inPipe;
    char *outPipe;
    char *inPipeArgs[MAX_ARGS/2];
    char *outPipeArgs[MAX_ARGS/2];
} Pipe;

struct d_struct {
    char *comm;
    char *argv[MAX_ARGS/2];
} dStruct;

const char *people[] = {
    "Sang Mercado",
    "Jared Pruett",
    "Mason Egger",
    "Derrick Green",
    NULL
};

char *shellArgv[MAX_ARGS];
char buffer[MAX_SIZE];
int bufferCount;
int shellArgc;
char userInput;

#endif
