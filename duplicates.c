//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

#include <unistd.h>
#include <getopt.h>
#include "duplicates.h"

#define OPTLIST "aAf:h:lq"

// Report the program's synopsis, should command-line processing detect any errors.
static void usage(char *program_name)
{
    printf("Usage: %s [options] <directory...>\n", program_name);
    printf("-a\t\tinclude directory entries whose names begin with a dot (.)\n"
           "-A\t\treport if the program attemps the advanced version\n"
           "-f=FILENAME\tlist all files whose SHA2 hash matches that of the indicated file\n"
           "-h=HASH\t\tlist all files with the indicated SHA2 hash\n"
           "-l\t\tlist all duplicates files\n"
           "-q\t\tquietly test if any duplicates files exist\n\n");
}

int main(int argc, char *argv[])
{
    char *program_name = argv[0];
    int opt;

    bool a_flag = false;
    bool f_flag = false;
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
    argv += optind;
    argc -= optind;

    if(argc < 1)
    {
        usage(program_name);
        exit(EXIT_FAILURE);
    }

    /* 
     * Processes all directories and add them to the hashtable.
     * Each hashtable entry represents a file and it's duplicates.
     */
    hashtable = hashtable_new();
    int nfiles_duplicate = 0;
    for(int i = 0; i < argc; i++)
        nfiles_duplicate += process_directory(argv[i], a_flag);

//  Handle the non-empty command-line options that aren't -a.
    if(f_flag)
    {
//      Test existence of the provided file.
        if(access(arg_str, F_OK) != 0)
            exit(EXIT_FAILURE);

        char *hash = strSHA2(arg_str);
        bool result = find_file(arg_str, hash);

        free(hashtable);

        if(result)
            exit(EXIT_SUCCESS);
        else
            exit(EXIT_FAILURE);
    }
    else if(h_flag)
    {
        bool result = find_file(NULL, arg_str);

        free(hashtable);

        if(result)
            exit(EXIT_SUCCESS);
        else
            exit(EXIT_FAILURE);
    }
    else if(l_flag)
    {
        list_duplicates();
    }
    else if(q_flag)
    {
        free(hashtable);

        if(nfiles_duplicate == 0)
            exit(EXIT_SUCCESS);
        else
            exit(EXIT_FAILURE);
    }
    else
    {
        report_statistics();
    }

    free(hashtable);
    
    exit(EXIT_SUCCESS);
}
