//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#define OPTLIST "aAf:h:lq"

void usage(int arg)
{
    if (arg == 1) printf("Something mysterious happens.\n");
}

int main(int argc, char *argv[])
{
    int opt;
    char *fname = NULL;
    char *hashbrown = NULL;

//  I literally have no idea what this is for but it seems important?.
//  opterr = 0;
    while((opt = getopt(argc, argv, OPTLIST)) != -1) 
    {
        if(opt == 'a')
        {
            printf("a works.\n");
        }
        else if(opt == 'A')
        {
            printf("A works.\n");
        }
        else if(opt == 'f')
        {
            fname = strdup(optarg);
            printf("f works. You typed: %s.\n", fname);     
        }
        else if(opt == 'h')
        {
            hashbrown = strdup(optarg);
            printf("h works. You typed: %s.\nNOTE: h CANNOT HANDLE HASH YET AND IS SUBSTITUTED WITH STRING.\n", hashbrown);
        }
        else if(opt == 'l')
        {
            printf("l works.\n");
        }
        else if(opt == 'q')
        {
            printf( "q works.\n");
        }
//  UNKNOWN ARGUMENT, SO argc SET TO -1 SO WE TRIGGER USAGE
        else
        {
            argc = -1;
        }
    }

    if(argc <= 0) usage(1);

//  Also not sure what these two do but they seem important?
//  argc -= optind;
//  argv += optind;
    exit(0);
}
