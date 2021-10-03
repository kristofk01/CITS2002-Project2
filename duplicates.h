#include <stdio.h>

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

typedef struct 
{
    char *name;
    char *hash;
    char *path;
    int   size;
} FILES;