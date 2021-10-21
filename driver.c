#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "duplicates.h"

static int keys[HASHTABLE_SIZE];
static int nkeys = 0;

/*
   Function that checks if the key specified
   exists yet in the hashtable. If not, the key
   is added.
   @param k represents the key being checked.
   @param *keys represents the hashtable.
*/
static void add_key(int *keys, int k)
{
//  dont't do anything if the key already exists
//  in the hashtable
    for(int i = 0; i < nkeys; ++i)
        if(k == keys[i])
            return;

    keys[nkeys++] = k;

//  Count the size and number of all unique files.
    statistics.total_size_unique += hashtable[k]->file.size;
    statistics.nfiles_unique = nkeys;
}

/* Scan a directory and its subdirectories, adding all files, 
   unique or not, to the hashtable.
   @param *hashtable is the hashtable.
   @param *dirname is the relative filepath of the directory.
   @param a_flag checks if -a is the inputted command-line option.
*/
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

//      if the current entry is a file, add it to the hashtable.
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
   Used to handle both -f and -h command-line options.
   Given a filename and a hash, it finds all files with
   that hash (in the case of -h) or all non-input filename
   with the same hash as the input filename (in the case of -f).
   All found files are then printed.
   @param filename is the input filename, which is always NULL 
   in the case of -h.
   @param hash is the input hash.
   @returns a boolean. False means no duplicates found in the
   case of -f and no files with the specified hash in the case
   of -h. 
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
    
// Traverse any duplicates the file may have.
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

    free(result);

    return true;
}

/* 
   List all files that are duplicates of at least one other file.
   Used when -l is the command-line option
*/
void list_duplicates()
{
    for(int i = 0; i < nkeys; ++i)
    {
        int k = keys[i];

//      Used to keep track of the number of duplicates file k has.
        int this_files_duplicate_count = 0;

        char buffer[4096];
        sprintf(buffer, "%s\t", hashtable[k]->file.name);

//      Look through for other files that, if exist, are then duplicates of k.
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
   Function that calls scan_directory() to process through 
   files in a directory.
   @param *dirname is the relative file path of the specified
   directory.
   @param a_flag checks whether or not -a was the inputted
   command-line option.
   @returns an int representing the number of duplicate files
   in the hashtable. If 0, there are no duplicate files.
*/
int process_directory(char *dirname, bool a_flag)
{
    scan_directory(hashtable, dirname, a_flag);
    return (statistics.nfiles - statistics.nfiles_unique);
}

/*
   Function that prints out the statistics of a directory
   (no. of unique files, no. of files, size of unique files,
   size of all files).
*/
void report_statistics()
{
    //This stuff's just for checking Advanced Task 2. TODO: REMOVE
    for(int i = 0; i < nkeys; i++)
    {
        printf("File:%s, Inode:%u, Size:%u\n", hashtable[keys[i]]->file.name, hashtable[keys[i]]->file.inode,hashtable[keys[i]]->file.size);
        LIST *current = hashtable[keys[i]]->next;
        while(current != NULL)
        {
            printf("File:%s, Inode:%u, Size:%u\n",current->file.name, current->file.inode, current->file.size);
            current = current->next;
        }
    }
    //////////////////////////////////////////////////////////////
    

    printf("Number of files:        "); printf("%u\n", statistics.nfiles);
    printf("Total size of files:    "); printf("%u\n", statistics.total_size);
    printf("Number of unique files: "); printf("%u\n", statistics.nfiles_unique);
    printf("Total size of unique:   "); printf("%u\n", statistics.total_size_unique);
}
