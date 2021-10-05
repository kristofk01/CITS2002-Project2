#include <stdlib.h>

#if defined(__linux__)
    extern char *strdup(const char *string);
#endif

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

typedef struct _D_FILE
{
    char    *name; // path
    char    *hash;
    int     size;
    struct _D_FILE  *parent;
    //int *parent;
} D_FILE;
