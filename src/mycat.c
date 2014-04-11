#include "mycat.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*int mycat(char *fileName)
{
    // Opening a new file stream
    FILE *file = fopen(fileName, "r");
    char buffer[MAX_SIZE];

    // Error Checking, perror will display ERRNO as string
    if (file == NULL) {
        // Using strerror instead of perror due to formatting reasons
        printf("mycat: %s: %s\n", fileName, strerror(errno));
        return -1;
    }
    
    // Might have to refactor this part.
    while((fgets(buffer, MAX_SIZE, file)) != NULL)
        printf("%s", buffer);
    fclose(file);
    return 0;
}
*/
int mycat(char *fileName)
{
    int fd = open(fileName, O_RDONLY);
    int bytesRead;
    char buffer[MAX_SIZE];

    // Error Checking, perror will display ERRNO as string
    if (!fd) {
        // Using strerror instead of perror due to formatting reasons
        printf("mycat: %s: %s\n", fileName, strerror(errno));
        return -1;
    }

    while ((bytesRead = read(fd, buffer, MAX_SIZE)) > 0) {
        printf("%s", buffer);
    }
}
