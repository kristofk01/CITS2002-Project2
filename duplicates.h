#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#if defined(__linux__)
    extern char *strdup(const char *string);
#endif

#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

// we chose to use a number that is a power of two
// instead of using a large prime as it gave us better
// performance
#define HASHTABLE_SIZE (1<<28)

typedef struct _d_file
{
    char        *name; // path
    char        *hash;
    uint32_t    size;
    int         inode;
} D_FILE;

typedef struct _list
{
    D_FILE         file;
    struct _list    *next;
} LIST;

typedef LIST *HASHTABLE;
HASHTABLE *hashtable;

struct STATS
{
    uint32_t nfiles;
    uint32_t nfiles_unique;
    uint32_t total_size;
    uint32_t total_size_unique;
};

struct STATS statistics;

extern HASHTABLE *hashtable_new(void);
extern int hashtable_add(HASHTABLE *, D_FILE new_file);
extern LIST *hashtable_find(HASHTABLE *, char *str);

extern char *strSHA2(char *filename);

extern int process_directory(char *dirname, bool a_flag);
extern bool find_file(char *filename, char *hash);
extern void list_duplicates();
extern void report_statistics();
