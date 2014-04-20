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
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stddef.h>
#include <stdlib.h>

static int SHOW_HIDDEN_FILES = 0;
static int SHOW_LONG_FORMAT = 0;

#define BUFFER_SIZE 512

int 
exclude(const struct dirent *a)
{
    return (strcmp(a->d_name, ".") && strcmp(a->d_name, "..") 
        && a->d_name[0] != '.');
}

char *elimChar(char *src, char target)
{
    char* index;
    char* returnStr = (char *)malloc(BUFFER_SIZE*sizeof(char));
    index = strchr(src, target);
    if (index != NULL) {
        strncpy(returnStr, index+1, strlen(src));
        return returnStr;
    }
    else return src;
}

void 
printFileNormal(char *fileName)
{
    fileName = elimChar(fileName, '/');
    fprintf(stdout, "%s\t", fileName);
}

void 
printFileLongFormat(char *fileName)
{
    struct stat attr;
    struct passwd *uid;
    struct group *gid;
    char buffer[BUFFER_SIZE];
    char link[BUFFER_SIZE];

    if (lstat(fileName, &attr) < 0) {
        fprintf(stdout, "myls: %s: %s\n", fileName, strerror(errno));
        return;
    }
    uid = getpwuid(attr.st_uid);
    gid = getgrgid(attr.st_gid);
    strftime(buffer, sizeof(buffer), "%b %d %R", localtime(&attr.st_atime));

    fprintf(stdout, (S_ISDIR(attr.st_mode)) ? "d" : (S_ISLNK(attr.st_mode)) ? "l" : "-");
    fprintf(stdout, (attr.st_mode & S_IRUSR) ? "r" : "-");
    fprintf(stdout, (attr.st_mode & S_IWUSR) ? "w" : "-");
    fprintf(stdout, (attr.st_mode & S_IXUSR) ? "x" : "-");
    fprintf(stdout, (attr.st_mode & S_IRGRP) ? "r" : "-");
    fprintf(stdout, (attr.st_mode & S_IWGRP) ? "w" : "-");
    fprintf(stdout, (attr.st_mode & S_IXGRP) ? "x" : "-");
    fprintf(stdout, (attr.st_mode & S_IROTH) ? "r" : "-");
    fprintf(stdout, (attr.st_mode & S_IWOTH) ? "w" : "-");
    fprintf(stdout, (attr.st_mode & S_IXOTH) ? "x" : "-");

    fprintf(stdout, " %-2d %-8s %-8s %-4d %-12s %s", (int)attr.st_nlink, uid->pw_name, 
        gid->gr_name, (int)attr.st_size, buffer, fileName);
    if (S_ISLNK(attr.st_mode)) {
        readlink(fileName, link, BUFFER_SIZE);
        link[BUFFER_SIZE-1] = '\0';
        fprintf(stdout, "-> %s", link);
    }
    fprintf(stdout, "\n");
}

int 
myls(char *fileName)
{
    struct stat attr;
    struct dirent **dirList;
    char buffer[BUFFER_SIZE];
    int i, n = 0;

    if (stat(fileName, &attr) < 0) {
        fprintf(stdout, "myls: %s: %s\n", fileName, strerror(errno));
        return -1;
    }
    if (S_ISDIR(attr.st_mode)) {
        int (*excludeFunc)(const struct dirent *a);
        SHOW_HIDDEN_FILES ? (excludeFunc = NULL) : (excludeFunc = exclude);
        n = scandir(fileName, &dirList, excludeFunc, alphasort);
    }
    void (*printFile)(char *fileName);
    SHOW_LONG_FORMAT ? (printFile = printFileLongFormat) : (printFile = printFileNormal);
    if (n == 0)
        printFile(fileName);
    else {
        chdir(fileName);
        for (i = 0; i < n; ++i)
            printFile(dirList[i]->d_name);
        chdir("..");
    }

    if (printFile == printFileNormal) fprintf(stdout, "\n");
    return 0;
}

int 
main(int argc, char *argv[])
{
    char *fileName;
    int i, ch;

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

    if (optind == argc)
        myls(".");

//   if (argc <= 1 || argv[argc-1][0] == '-')
//        myls(".");

    for (i = argc-1; i > 0; --i)
        if (argv[i][0] != '-')
            myls(argv[i]);

    return 0;
}

