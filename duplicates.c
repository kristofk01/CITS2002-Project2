//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

// TODO: at the end, figure out what header files are needed for most .c files
// and move them to duplicates.h

#include <unistd.h>
#include <getopt.h>
#include <dirent.h>

#include <sys/param.h>
#include <sys/stat.h>

#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

extern char *strSHA2(char *filename);

HASHTABLE *hashtable;

// A duplicate file is defined as having a parent who is not itself.
// In other words, unique files are the parents of themselves.
/*
void identify_duplicates(void)
{
    int j = 0;
    while(j < nfiles)
    {
        if(files[j].parent == NULL) // if we have not yet considered file j, then...
        {
            // grab its hash and find any files with equivalent hash
            char *target_hash = files[j].hash;
            for(int i = 0; i < nfiles; ++i)
            {
                // if the file has the same hash and is an orphan (not yet considered)
                if(strcmp(target_hash, files[i].hash) == 0 && files[i].parent == NULL)
                {
                    // set its parent
                    files[i].parent = &files[j];
                }
            }
        }
        ++j;
    }
}
*/

void scan_directory(char *dirname, bool a_flag)
{
    DIR *dir = opendir(dirname);
    struct dirent *entry = NULL;

    if(dir == NULL)
    {
        // TODO: check if this is how they actually want us to handle errors
        // in the project spec...
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
                hashtable_add(hashtable, file);
            }
            else if(!a_flag && strncmp(entry->d_name, ".", 1))
            {
                hashtable_add(hashtable, file);
            }
        }
        else if(S_ISDIR(statinfo.st_mode))
        {
            // the current entry is a directory so traverse it
            scan_directory(pathname, a_flag);
        }
    }

    closedir(dir);
}

/*
void compute_statistics(int *nfiles_unique, int *total_size_unique, int *total_size)
{
    for (int i = 0; i < nfiles; i++)
    {
        if (strcmp(files[i].name, files[i].parent->name) == 0)
        {
            *nfiles_unique += 1;
            *total_size_unique += files[i].size;
        }
        *total_size += files[i].size;
    }
}
*/

void usage(char *program_name)
{
    // TODO: remove m flag if we don't actually end up implementing it
    printf("Usage: %s [options] <directory...>\n", program_name);
    printf("-a\t\tinclude directory entries whose names begin with a dot (.)\n"
           "-A\t\treport if the program attemps the advanced version\n"
           "-f=FILENAME\tlist all files whose SHA2 hash matches that of the indicated file\n"
           "-h=HASH\t\tlist all files with the indicated SHA2 hash\n"
           "-l\t\tlist all duplicates files\n"
           "-m\t\tminimizes the total number of bytes required to store all files' data\n"
           "-q\t\tquietly test if any duplicates files exist\n\n");
}

int main(int argc, char *argv[])
{
    char *program_name = argv[0];
    int opt;

    // these booleans are getting ridiculous, figure out another way
    bool a_flag = false; // for -a
    //bool q_flag = false;
    //bool l_flag = false;
    
    char *fname = NULL;
    char *hash_str = NULL;

    while((opt = getopt(argc, argv, OPTLIST)) != -1) 
    {
        switch(opt)
        {
            case 'a':
                a_flag = true;
                break;

            case 'A':
                printf("A works.\n");
                break;

            case 'f':
                fname = strdup(optarg);
                printf("f works. You typed: %s.\n", fname);
                break;
            case 'h':
                hash_str = strdup(optarg);
                printf("h works. You typed: %s.\nNOTE: h CANNOT HANDLE HASH YET AND IS SUBSTITUTED WITH STRING.\n", hash_str);
                break;

            case 'l':
                //l_flag = true;
                break;

            case 'q':
                //q_flag = true;
                break;

            default:
                argc = -1;
                break;
        }
    }

    if(argc <= 1) usage(program_name);

    hashtable = hashtable_new();

//  OPEN AND PROCESS DIRECTORIES
    for(int i = optind; i < argc; i++)
        scan_directory(argv[i], a_flag);

//  IDENTIFY DUPLICATES


/*
//#define _DEBUG_PROJECT_
#ifdef _DEBUG_PROJECT_
    printf("No.\t\tSize\tFilename\t\tHash\n");
    for(int i = 0; i < nfiles; ++i) {
        printf("File %i: \t%i\t%s\t%s\t%p\n",
            i, files[i].size, files[i].name, files[i].hash, (void *)files[i].parent);
    }
#endif

    identify_duplicates();

#ifdef _DEBUG_PROJECT_
    printf("\nParent\t\t\t\tFilename\n");
    for(int i = 0; i < nfiles; ++i) {
        D_FILE *parent = files[i].parent;
        //printf("%p\n", (void *)parent);
        if(parent != NULL)
            printf("%s \t\t%s\n", parent->name, files[i].name);
    }
    printf("\n");
#endif

    int nfiles_unique = 0;
    int total_size_unique = 0;
    int total_size = 0;
    compute_statistics(&nfiles_unique, &total_size_unique, &total_size);

// HANDLE -l
    if(l_flag)
    {
        bool seen[nfiles];
        int j = 0;

        while(j < nfiles)
        {
            char buffer[MAXPATHLEN];
            char big_buffer[MAXPATHLEN * nfiles];
            int count = 0;
            // if we are looking at something that is a parent of itself
            if (strcmp(files[j].name, files[j].parent->name) == 0)
            {
                sprintf(buffer, "%s\t", files[j].name);
                //printf("\t%s\n", buffer);
                //seen[j] = true;
                ++j;
                continue;
            }

            for(int i = 0; i < nfiles; ++i)
            {
                // if we have not checked this file and it has a parent other than itself
                // then we know its a duplicate of the jth file...
                if(!seen[i] && strcmp(files[j].name, files[i].parent->name) != 0)
                {
                    strcat(big_buffer, files[i].name);
                    strcat(big_buffer, "\t");
                    //printf("%s\n", big_buffer);
                    seen[i] = true;
                    ++count;
                }
            }
            
            if(count > 1)
            {
                char *result = malloc((MAXPATHLEN * nfiles) * sizeof(char *));
                CHECK_ALLOC(result);

                strcat(result, buffer);
                strcat(result, big_buffer);
                printf("%s\n", result);

                free(result);
            }
            ++j;
        }
    }

// HANDLE -q
    if(q_flag)
    {
        if(nfiles == nfiles_unique)
        {
            printf("EXIT_SUCCESS (remember to remove this later).\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("EXIT_FAILURE (remember to remove this later).\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(!q_flag && !l_flag) // report statistics
    {
        // TODO: remove left printf column
        printf("Total files:\t");           printf("%u\n", nfiles);
        printf("Total size:\t");            printf("%u\n", total_size);
        printf("Total unique files:\t");    printf("%u\n", nfiles_unique);
        printf("Total min. size:\t");       printf("%u\n", total_size_unique);
    }
    free(files);
    */

    exit(EXIT_SUCCESS);
}
