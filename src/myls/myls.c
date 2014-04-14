// For ls
//  if file, get stat of the file and display it the way ls does
// if directory, scan directory, and do it again

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int myls(char *fileName)
{
    struct stat attr;

    // This can be refactored as mycat eventually
    if (stat(fileName, &attr) < 0)
        return -1;
    if (S_ISDIR(attr.st_mode)) {
        printf("is directory");
    }
    else {
        printf("%s", fileName);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *fileName;
    if (argc == 1) myls(".");
    else {
        int i;
        for (i = 0; i < argc; ++i)
            myls(argv[i+1]);
    }
    printf("\n");
    return 0;
}

