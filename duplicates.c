//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

#include <unistd.h>
#include <getopt.h>
#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

/* Used to check how fast the function runs when no command-line
   option or -a is entered.
#include <time.h>
*/

/* 
   Function to report the program's synopsis, should command-line 
   processing detect any errors.
   @param *program_name is the faulty argument input.
*/
static void usage(char *program_name)
{
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
/* 
    Used to check how fast the function runs when no command-line
    option or -a is entered.
 
    clock_t start, end;
    start = clock();
*/
    
    char *program_name = argv[0];
    int opt;

    bool a_flag = false; // booleans that determine which command-line option
    bool f_flag = false; // was inputted.
    bool h_flag = false;
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
                exit(EXIT_SUCCESS);
                break;

            case 'f':
                arg_str = strdup(optarg);
                f_flag = true;
                break;

            case 'h':
                arg_str = strdup(optarg);
                h_flag = true;
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

/* 
   Processes all directories and adds them to a 
   hashtable. Every unique 'key' in the hashtable 
   represents a unique hash, and duplicates are 
   added to their corresponding hashtable locations.
*/
    hashtable = hashtable_new();
    int nfiles_duplicate = 0;
    for(int i = optind; i < argc; i++)
    {
        nfiles_duplicate += process_directory(argv[i], a_flag);
    }

    // DEBUG : REMOVE
    /*
    for(int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        LIST *current = hashtable[i];
        if(current == NULL) continue;

        printf("[%i] %s\n", i, current->file.name);
        while((current = current->next) != NULL)
        {
            printf("\t -> %s\n", current->file.name);
        }
        printf("###########\n\n");

    }
    */
    /////////////////

//  Handles the non-empty command-line options that aren't -a.
    if(f_flag)
    {
//      Test existence of the provided file.
        if(access(arg_str, F_OK) != 0)
        {
            printf("EXIT_FAILURE (remember to remove this later).\n");
            exit(EXIT_FAILURE);
        }

        char *hash = strSHA2(arg_str);
        bool result = find_file(arg_str, hash);

        free(hashtable);

        if(result)
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
    else if(h_flag)
    {
        bool result = find_file(NULL, arg_str);

        free(hashtable);

        if(result)
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
    else if(l_flag)
    {
        list_duplicates();
    }
    else if(q_flag)
    {
        free(hashtable);

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
    else
    {
        report_statistics();
    }

    free(hashtable);
    
/* 
    Used to check how fast the function runs when no command-line
    option or -a is entered.

    end = clock();
    printf("(Whatever) sized hashtable took %f seconds to execute\n", ((double) (end - start))/CLOCKS_PER_SEC);
*/
    
    exit(EXIT_SUCCESS);
}
