#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h>

#if defined(__linux__)
    extern char *strdup(const char *string);
#endif

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

//  --------------------------------------------------------------------

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

typedef LIST *HASHTABLE;

extern HASHTABLE *hashtable_new(void);
extern int hashtable_add(HASHTABLE *, D_FILE new_file);
extern LIST *hashtable_find(HASHTABLE *, char *str);
extern uint32_t hash_string(char *string);

extern char *strSHA2(char *filename);

extern int process_directory(char *dirname, bool a_flag);
extern bool find_file(char *hash);
extern void list_duplicates();
extern void report_statistics();
