/* Contains global variables as welll as
 * custom datatypes for pipe, redirection,
 * and $()
 **/ 

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

/* Custom Structs for different scenarios
 * such as Pipes, Redirects, $()
 * They are defined as global variables
 **/

struct RedirectStruct {
    char *file;
    short oFlags;
    short input;
    short output;
} Redirect;

struct PipeStruct {
    int pipe[2];
    char *inPipe;
    char *outPipe;
    char *inPipeArgs[MAX_ARGS/2];
    char *outPipeArgs[MAX_ARGS/2];
} Pipe;

struct DollarStruct {
    char *comm;
    char *argv[MAX_ARGS/2];
} Dollar;

struct passwd *user;

/* Start of global variables that are used throughout the program */

const char *people[] = {
    "Sang Mercado",
    "Jared Pruett",
    "Mason Egger",
    "Derrick Green",
    NULL
};

/* These variables are reset at a function in implementation under 
 * destroyBuffer()
 **/

char *shellArgv[MAX_ARGS];
char buffer[MAX_SIZE];
int bufferCount;
int shellArgc;
char userInput;

#endif
