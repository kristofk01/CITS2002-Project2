//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

#include <unistd.h>
#include <getopt.h>
#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

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

    // NOTE: these booleans are getting ridiculous, figure out another way
    bool a_flag = false; // for -a
    bool fh_flag = false;
    bool q_flag = false;
    bool l_flag = false;
    
    char *arg_str = NULL;

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
                arg_str = strSHA2(strdup(optarg));
                fh_flag = true;
                break;

            case 'h':
                arg_str = strdup(optarg);
                fh_flag = true;
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
        exit(EXIT_FAILURE);
    }

    HASHTABLE *hashtable = hashtable_new();

// OPEN AND PROCESS DIRECTORIES
    for(int i = optind; i < argc; i++)
        scan_directory(hashtable, argv[i], a_flag);

    // statistics
    int nfiles_unique = 0;
    int nfiles_duplicate = 0;
    int total_size_unique = 0;
    int total_size_duplicate = 0;

// IDENTIFY DUPLICATES
    int *unique_inodes = NULL;

    // is looping through the entire hashtable the best solution???
    for(int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        if(hashtable[i] != NULL)
        {
            // ADVANCED TASK 2... //
            struct stat statinfo;
            char *pathname = hashtable[i]->file.name;
            if(stat(pathname, &statinfo) != 0)
            {
                perror(pathname);
                exit(EXIT_FAILURE);
            }

            int size = nfiles_unique + nfiles_duplicate + 1;
            unique_inodes = realloc(unique_inodes, sizeof(int) * size);
            CHECK_ALLOC(unique_inodes);

            int current_ino = statinfo.st_ino;

            for(int i = 0; i < size; ++i)
                if(unique_inodes[i] == current_ino)
                    continue;

            unique_inodes[size-1] = current_ino;

            /////////////////////////

            // to keep track of the number of duplicates file i has
            int this_files_duplicate_count = 0;

            // NOTE: is this an okay size?
            char buffer[4096];
            sprintf(buffer, "%s\t", hashtable[i]->file.name);

            // look through for other files that, if exist, are then duplicates of i
            LIST *current = hashtable[i]->next;
            while(current != NULL)
            {
                strcat(strcat(buffer, current->file.name), "\t");

                total_size_duplicate += current->file.size;
                ++this_files_duplicate_count;

                current = current->next;
            }

// HANDLE -l
            if(l_flag && this_files_duplicate_count >= 1)
            {
                int len = strlen(buffer);
                buffer[len] = '\0';
                printf("%s\n", buffer);
            }
            
            ++nfiles_unique;
            total_size_unique += hashtable[i]->file.size;
            nfiles_duplicate += this_files_duplicate_count;
        }
    }

    free(unique_inodes);

// HANDLE -f AND -h
    if(fh_flag)
    {
        if(arg_str == NULL)
        {
            printf("EXIT_FAILURE (remember to remove this later).\n");
            exit(EXIT_FAILURE);
        }

        int count = 0;
        uint32_t h = hash_string(arg_str) % HASHTABLE_SIZE;

        // NOTE: is this an okay size?
        char buffer[4096];
        sprintf(buffer, "%s\n", hashtable[h]->file.name);

        // traverse any duplicates the file may have
        LIST *current = hashtable[h]->next;
        while(current != NULL)
        {
            strcat(strcat(buffer, current->file.name), "\n");
            current = current->next;
            ++count;
        }

        int len = strlen(buffer);
        buffer[len] = '\0';
        printf("%s", buffer);

        free(hashtable);

        if(count > 0)
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

    free(hashtable);

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
    else if(!q_flag && !l_flag && !fh_flag) // report statistics
    {
        // TODO: remove left printf column
        printf("Total files:\t\t");         printf("%u\n", nfiles_unique + nfiles_duplicate);
        printf("Total size:\t\t");          printf("%u\n", total_size_unique + total_size_duplicate);
        printf("Total unique files:\t");    printf("%u\n", nfiles_unique);
        printf("Total min. size:\t");       printf("%u\n", total_size_unique);
    }

    exit(EXIT_SUCCESS);
}
