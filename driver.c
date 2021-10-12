#include <dirent.h>
#include <sys/param.h>

#include "duplicates.h"

void scan_directory(HASHTABLE *hashtable, char *dirname, bool a_flag)
{
    DIR *dir = opendir(dirname);
    struct dirent *entry = NULL;

    if(dir == NULL)
    {
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
            scan_directory(hashtable, pathname, a_flag);
        }
    }

    closedir(dir);
}