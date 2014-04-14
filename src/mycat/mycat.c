#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define MAX_SIZE 2048

int mycat(FILE *fp, char *fileName)
{
    if (!fp) {
        printf("mycat: %s: %s\n", fileName, strerror(errno));
        return -1;
    }
    char buffer[MAX_SIZE];
    char *n;
    while ((n = fgets(buffer, MAX_SIZE, fp)) != NULL) {
        fprintf(stdout, buffer);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) mycat(stdin, NULL);
    else {
        int i;
        for (i = 0; i < argc - 1; ++i) {
            FILE *fp = fopen(argv[i+1], "r");
            mycat(fp, argv[1]);
        }
    }
    return 0;
}
