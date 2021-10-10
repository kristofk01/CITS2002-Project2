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
    bool q_flag = false;
    bool l_flag = false;
    
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
                exit(EXIT_FAILURE); // TODO: change
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
                l_flag = true;
                break;

            case 'q':
                q_flag = true;
                break;

            default:
                argc = -1;
                break;
        }
    }

    if(argc <= 1)
    {
        usage(program_name);
        exit(EXIT_FAILURE); // or success? 
    }

    hashtable = hashtable_new();

//  OPEN AND PROCESS DIRECTORIES
    for(int i = optind; i < argc; i++)
        scan_directory(argv[i], a_flag);

//  IDENTIFY DUPLICATES
    int nfiles_unique = 0;
    int nfiles_duplicate = 0;
    int total_size_unique = 0;
    int total_size = 0;

    // is looping through the entire hashtable the best solution???
    //NOTE: there is a bug where total_size is counted incorrectly!

    for(int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        if(hashtable[i] != NULL)
        {
            // to keep track of the number of duplicates file i has
            int this_files_duplicate_count = 0;

            // NOTE: is this an okay size?
            char buffer[4096];
            sprintf(buffer, "%s\t", hashtable[i]->file.name);

            // look through for other files that, if exist, are then unique
            LIST *next = hashtable[i]->next;
            while(next != NULL)
            {
                strcat(strcat(buffer, next->file.name), "\t");

                total_size += next->file.size;
                ++this_files_duplicate_count;

                next = next->next;
            }

// HANDLE -l
            // TODO: add '\0' somehow to buffer...
            if(l_flag && this_files_duplicate_count >= 1)
                printf("%s\n", buffer);
            
// UPDATE STATISTICS
            ++nfiles_unique;
            total_size_unique += hashtable[i]->file.size;
            total_size += total_size_unique;
            nfiles_duplicate += this_files_duplicate_count;
        }
    }

// HANDLE -q
    if(q_flag)
    {
        if(nfiles_duplicate == 0)
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
        printf("Total files:\t\t");         printf("%u\n", nfiles_unique + nfiles_duplicate);
        printf("Total size:\t\t");          printf("%u\n", total_size);
        printf("Total unique files:\t");    printf("%u\n", nfiles_unique);
        printf("Total min. size:\t");       printf("%u\n", total_size_unique);
    }
   
    free(hashtable);

    exit(EXIT_SUCCESS);
}
