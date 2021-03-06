#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "duplicates.h"

static int keys[HASHTABLE_SIZE];
static int nkeys = 0;

/*
 * Keep track of the keys of our hashtable entries by assigning
 * each key to the unique array keys[]
 */
static void add_key(int *keys, int k)
{
//  Don't do anything if the key already exists in the hashtable.
    for(int i = 0; i < nkeys; ++i)
        if(k == keys[i])
            return;

    keys[nkeys++] = k;

//  Count the size and number of all unique files.
    statistics.total_size_unique += hashtable[k]->file.size;
    statistics.nfiles_unique = nkeys;
}

// Scan a directory and its subdirectories; adding all files to the hashtable.
static void scan_directory(HASHTABLE *hashtable, char *dirname, bool a_flag)
{
    DIR *dir = opendir(dirname);
    struct dirent *entry = NULL;

    if(dir == NULL)
    {
        perror(dirname);
        exit(EXIT_FAILURE);
    }

    while((entry = readdir(dir)) != NULL)
    {
        // Ignore . and .. entries
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        
        char pathname[MAXPATHLEN];
        sprintf(pathname, "%s/%s", dirname, entry->d_name);
        
        struct stat statinfo;
        if(stat(pathname, &statinfo) != 0)
        {
            perror(pathname);
            exit(EXIT_FAILURE);
        }

//      If the current entry is a file or a symbolic link, add it to the hashtable.
        if(S_ISREG(statinfo.st_mode))
        {
            D_FILE file;
            file.hash = strSHA2(pathname);
            file.name = strdup(pathname);
            file.size = statinfo.st_size;
            file.inode = statinfo.st_ino;

//          Handle -a.
            if(a_flag)
            {
                int k = hashtable_add(hashtable, file);
                add_key(keys, k);
            }
            else if(!a_flag && strncmp(entry->d_name, ".", 1))
            {
                int k = hashtable_add(hashtable, file);
                add_key(keys, k);
            }
        }
        else if(S_ISDIR(statinfo.st_mode))
        {
//          The current entry is a directory so traverse it.
            scan_directory(hashtable, pathname, a_flag);
        }
    }

    free(entry);
    closedir(dir);
}

/* 
 * Used to handle both -f and -h command-line options.
 * Find all files with the same hash as that of the given hash
 * or filename's hash. All found files are then printed.
 */
bool find_file(char *filename, char *hash)
{
    LIST *result = hashtable_find(hashtable, hash);

    if(result == NULL)
        return false;

    char buffer[4096];
    if(filename == NULL || strcmp(filename, result->file.name) != 0)
    {
        sprintf(buffer, "%s\n", result->file.name);
    }
    
//  Traverse any duplicates the file may have.
    LIST *current = result->next;
    if(current == NULL)
        return false;

    while(current != NULL)
    {
        if(filename == NULL || strcmp(filename, current->file.name) != 0)
        {
            strcat(strcat(buffer, current->file.name), "\n");
        }
        current = current->next;
    }

    int len = strlen(buffer);
    buffer[len] = '\0';
    printf("%s", buffer);

    return true;
}

/* 
 * List all files that are duplicates of at least one other file.
 * Used when -l is the command-line option
 */
void list_duplicates()
{
    for(int i = 0; i < nkeys; ++i)
    {
        int k = keys[i];

//      Keep track of the number of duplicates file k has.
        int this_files_duplicate_count = 0;

        char buffer[4096];
        sprintf(buffer, "%s\t", hashtable[k]->file.name);

//      Traverse any duplicates the file may have.
        LIST *current = hashtable[k]->next;
        while(current != NULL)
        {
            strcat(strcat(buffer, current->file.name), "\t");

            ++this_files_duplicate_count;

            current = current->next;
        }

        if(this_files_duplicate_count >= 1)
        {
            int len = strlen(buffer);
            buffer[len] = '\0';
            printf("%s\n", buffer);
        }
    }
}

/*
 * Function that calls scan_directory() to process through 
 * files in a directory. Returning the number of duplicate files found
 * per directory (dirname).
 */
int process_directory(char *dirname, bool a_flag)
{
    scan_directory(hashtable, dirname, a_flag);
    return (statistics.nfiles - statistics.nfiles_unique);
}

/*
 * Prints out the required statistics of a scanned directory
 */
void report_statistics()
{
    printf("%u\n", statistics.nfiles);
    printf("%u\n", statistics.total_size);
    printf("%u\n", statistics.nfiles_unique);
    printf("%u\n", statistics.total_size_unique);
}
