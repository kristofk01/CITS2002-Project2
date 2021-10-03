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
#include <sys/stat.h>

#include "duplicates.h"

#define OPTLIST "aAmf:h:lq"

extern char *strSHA2(char *filename);

// ::::::: THIS IS VERY VERY WORK IN PROGRESS :::::::
D_FILE *files = NULL;
int nfiles = 0;

void add_file(char *name, int size)
{
    files = realloc(files, (nfiles+1) * sizeof(D_FILE));
    CHECK_ALLOC(files);

    files[nfiles].name = strdup(name);
    files[nfiles].size = size;
    ++nfiles;
}
// ::::::::::::::::::::::::::::::::::::::::::::::::::

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
        
        struct stat statinfo;
        if(stat(pathname, &statinfo) != 0)
        {
            perror(pathname);
            exit(EXIT_FAILURE);
        }

        // if the current entry is a file
        if(entry->d_type == DT_REG)
        {
            // handle if we have used '-a' flag or not...
            if(all_flag)
            {
                add_file(pathname, statinfo.st_size);
            }
            else if(!all_flag && strncmp(entry->d_name, ".", 1))
            {
                add_file(pathname, statinfo.st_size);
            }
        }
        else if(entry->d_type == DT_DIR)
        {
            // if the current entry is a directory then traverse it
            scan_directory(pathname, all_flag);
        }
    }

    closedir(dir);
}

void usage(char *program_name)
{
    // TODO: remove m flag if we don't actually end up implementing it
    printf("Usage: %s [-aAm] [-l | -q] [-f filename | -h hash] <directory...>\n",
            program_name);
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

// ::::DEBUG::::
    for(int i = 0; i < nfiles; ++i) {
        printf("File %i: %s\t\t%i\n", i, files[i].name, files[i].size);
    }
// :::::::::::::

    free(files);

    exit(0);
}
