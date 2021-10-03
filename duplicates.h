#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

typedef struct 
{
    char *name;
    char *hash;
    char *path;
    int   size;
} FILES;