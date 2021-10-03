//  CITS2002 Project 2 2021
//  Name(s):             Kristof Kovacs , Daniel Ling
//  Student number(s):   22869854       , 22896002

// TODO: at the end, figure out what header files are needed for most .c files
// and move them to duplicates.h
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>

#include <sys/param.h>

#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

void usage(char *program_name)
{
    // TODO: remove m flag if we don't actually end up implementing it
    printf("Usage: %s [-aAm] [-l | -q] [-f filename] [-h hash] <directory...>\n",
            program_name);
}

void scan_directory(char *dirname, bool all_flag)
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

        // print everything OR print everything but the files starting with "."
        if(all_flag)
            printf("%s\n", pathname);
        else if(strncmp(entry->d_name, ".", 1))
            printf("%s\n", pathname);

        if(entry->d_type == DT_DIR)
            scan_directory(pathname, all_flag);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    char *program_name = argv[0];
    int opt;
    bool all_flag = false; // for -a
    char *fname = NULL;
    char *hashbrown = NULL;

//  I literally have no idea what this is for but it seems important?.
//  opterr = 0;
    while((opt = getopt(argc, argv, OPTLIST)) != -1) 
    {
        switch(opt)
        {
            case 'a':
                all_flag = true;
                break;

            case 'A':
                printf("A works.\n");
                break;

            case 'f':
                fname = strdup(optarg);
                printf("f works. You typed: %s.\n", fname);
                break;
            case 'h':
                hashbrown = strdup(optarg);
                printf("h works. You typed: %s.\nNOTE: h CANNOT HANDLE HASH YET AND IS SUBSTITUTED WITH STRING.\n", hashbrown);
                break;

            case 'l':
                printf("l works.\n");
                break;

            case 'q':
                printf( "q works.\n");
                break;

            default:
                argc = -1;
                break;
        }
    }

    if(argc <= 1) usage(program_name);

//  OPEN AND PROCESS DIRECTORIES
    for(; optind < argc; optind++)
        scan_directory(argv[optind], all_flag);

    exit(0);
}
