#include <sys/types.h>
#include <string.h>
#include <dirent.h>

int exclude(const struct dirent *a)
{
    return (strcmp(a->d_name, ".") && strcmp(a->d_name, ".."));
}

int mycat(char *source, char *dest)
{
    char buffer[2048];

    

    struct dirent **source

    int n = scandi
}
