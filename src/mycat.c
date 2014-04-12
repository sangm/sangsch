#include "mycat.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int mycat(char *fileName)
{
    int fd = open(fileName, O_RDONLY);
    int bytesRead;

    // MAX_SIZE comes from header file
    char buffer[MAX_SIZE];
    char errorMsg[MAX_SIZE]; 

    // Error Checking, perror will display ERRNO as string
    if (fd == -1) {
        // Using strerror instead of perror due to formatting reasons
        printf("mycat: %s: %s\n", fileName, strerror(errno));
        return -1;
    }

    while ((bytesRead = read(fd, buffer, MAX_SIZE)) > 0) 
        write(1, buffer, bytesRead);

    return 0;
    // There is another case where cat has to take care of two files

}

int main(int argc, char *argv[])
{
    mycat(argv[1]);
}
