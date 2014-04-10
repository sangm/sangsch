#include "mycat.h"

int mycat(char *fileName)
{
    // Opening a new file stream
    FILE *file = fopen(fileName, "r");
    char buffer[MAX_SIZE];

    // Error Checking, perror will display ERRNO as string
    if (file == NULL) {
        printf("mycat: %s: %s\n", fileName, strerror(errno));
        return -1;
    }
    
    while((fgets(buffer, MAX_SIZE, file)) != NULL)
        printf("%s", buffer);
    fclose(file);
    return 0;
}
