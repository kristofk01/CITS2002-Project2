#include <dirent.h>
#include <sys/param.h>

#include "duplicates.h"

struct STATS
{
    int nfiles_unique;
    int nfiles_duplicate;
    int total_size_unique;
    int total_size_duplicate;
};

struct STATS statistics;

HASHTABLE *hashtable;

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
        // ignore . and .. entries
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

        // if the current entry is a file, add it to the hashtable
        if(S_ISREG(statinfo.st_mode))
        {
            D_FILE file;
            file.hash = strSHA2(pathname);
            file.name = strdup(pathname);
            file.size = statinfo.st_size;

            // handle -a
            if(a_flag)
            {
                statistics.nfiles_duplicate = hashtable_add(hashtable, file);
            }
            else if(!a_flag && strncmp(entry->d_name, ".", 1))
            {
                statistics.nfiles_duplicate = hashtable_add(hashtable, file);
            }
        }
        else if(S_ISDIR(statinfo.st_mode))
        {
            // the current entry is a directory so traverse it
            scan_directory(hashtable, pathname, a_flag);
        }
    }

    free(entry);
    closedir(dir);
}

bool find_file(char *hash, char *filename)
{
    int count = 0;
    LIST *result = hashtable_find(hashtable, hash);

    if(result == NULL)
        return false;

    char buffer[4096];
    sprintf(buffer, "%s\n", result->file.name);

    // traverse any duplicates the file may have
    LIST *current = result->next;
    while(current != NULL)
    {
        strcat(strcat(buffer, current->file.name), "\n");
        current = current->next;
        ++count;
    }

    int len = strlen(buffer);
    buffer[len] = '\0';
    printf("%s", buffer);

    free(result);
    free(current);

    if(count > 0)
        return true;

    return false;
}

int process_directory(char *dirname, bool a_flag)
{
    hashtable = hashtable_new();

    scan_directory(hashtable, dirname, a_flag);

    printf("\tHashtable Entries (first only):\n");
    for(int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        if(hashtable[i] != NULL)
            printf("\t\t%s\n", hashtable[i]->file.name);
    }

    free(hashtable);

    return statistics.nfiles_duplicate;
}

void report_statistics()
{
    printf("%i\n", statistics.nfiles_duplicate);
    // TODO: remove left printf column
    /*
    printf("Total files:\t\t");         printf("%u\n", nfiles_unique + nfiles_duplicate);
    printf("Total size:\t\t");          printf("%u\n", total_size_unique + total_size_duplicate);
    printf("Total unique files:\t");    printf("%u\n", nfiles_unique);
    printf("Total min. size:\t");       printf("%u\n", total_size_unique);
    */
}