// For ls
//  if file, get stat of the file and display it the way ls does
// if directory, scan directory, and do it again

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

static int SHOW_HIDDEN_FILES = 0;
static int SHOW_LONG_FORMAT = 0;

int 
exclude(const struct dirent *a)
{
    return (strcmp(a->d_name, ".") && strcmp(a->d_name, "..") 
        && a->d_name[0] != '.');
}

void 
printDirectory(struct dirent *dirList[], int size)
{
    int i;
    for (i = 0; i < size; ++i)
        fprintf(stdout, "%s\t",dirList[i]->d_name);
}

void 
printFileNormal(char *fileName)
{
    fprintf(stdout, "%s\t", fileName);
}

int 
myls(char *fileName)
{
    struct stat attr;

    // This can be refactored as mycat eventually
    if (stat(fileName, &attr) < 0) {
        fprintf(stdout, "myls: %s: %s\n", fileName, strerror(errno));
        return -1;

    }
    if (S_ISDIR(attr.st_mode)) {
        struct dirent **dirList;
        int i, n;

        int (*excludeFunc)(const struct dirent *a);
        SHOW_HIDDEN_FILES ? (excludeFunc = NULL) : (excludeFunc = exclude);
        n = scandir(fileName, &dirList, excludeFunc, alphasort);
    
    // For 
        for (i = 0; i < n; ++i)
            printFileNormal(dirList[i]->d_name);

    }
    else {
        printf("%s ", fileName);
    }

    return 0;
}

int 
main(int argc, char *argv[])
{
    char *fileName;

    int ch;
    while ((ch = getopt(argc, argv, "al")) != -1) {
        switch(ch) {
            case 'a':
                SHOW_HIDDEN_FILES = 1;
                break;
            case 'l':
                SHOW_LONG_FORMAT = 1;
                break;
            default:
                break;
        }
    }

    int i;

    if (argc == 1) printf("hmm");

    for (i = argc-1; i > 0; --i) 
        myls(argv[i]);

    printf("\n");
    return 0;
}

