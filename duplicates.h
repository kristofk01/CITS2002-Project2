#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#if defined(__linux__)
    extern char *strdup(const char *string);
#endif

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

#define HASHTABLE_SIZE 997

typedef struct
{
    char    *name; // path
    char    *hash;
    int     size;
} D_FILE;

typedef struct _list
{
    D_FILE         file;
    struct _list    *next;
} LIST;
extern LIST *list_add(LIST *list, D_FILE new_file);
extern bool list_find (LIST *list, char *str);

typedef LIST *HASHTABLE;
extern HASHTABLE *hashtable_new(void);
extern void hashtable_add(HASHTABLE *, D_FILE new_file);
extern bool hashtable_find(HASHTABLE *, char *str);
extern uint32_t hash_string(char *string);
