//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

// TODO: at the end, figure out what header files are needed for most .c files
// and move them to duplicates.h
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

void usage(char *program_name)
{
    // TODO: remove m flag if we don't actually end up implementing it
    printf("Usage: %s [-aAm] [-l | -q] [-f filename] [-h hash] <directory...>\n",
            program_name);
}

int main(int argc, char *argv[])
{
    char *program_name = argv[0];
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

    if(argc <= 0) usage(program_name);

//  Also not sure what these two do but they seem important?
//  argc -= optind;
//  argv += optind;
    exit(0);
}
