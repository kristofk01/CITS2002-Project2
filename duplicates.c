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

#define OPLIST "aAmf:h:lq"

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
    D_FILE arg_file;

    if(argc <= 0) usage(program_name);

    exit(EXIT_SUCCESS);
}
