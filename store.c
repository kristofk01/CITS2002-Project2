#include "duplicates.h"

/*
   Function that traverses a linked list to see if
   any one of the files' hash or inode match
   with either the input hash or inode.
   @param *list is the traversed linked list.
   @param *str is the input hash.
   @param inode is the input inode.
   @returns are we returning the list? I can't tell dude
*/
static LIST *list_find(LIST *list, char *str, int inode)
{
    while(list != NULL)
    {
        if(strcmp(list->file.hash, str) == 0 ||
            list->file.inode == inode)
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

/*
   Function that reallocates memory to allow
   a file to be appended to the end of a linked list.
   @param file is the new file to be appended.
*/
static LIST *list_new_item(D_FILE file)
{
    LIST *new = malloc( sizeof(LIST) );
    CHECK_ALLOC(new);

    new->file = file;
    new->next = NULL;

    return new;
}


/*
   Given a file and a linked list, check that the
   file is not in the linked list. If so, append
   the file to the end of the linked list.
   @param *list is the checked linked list.
   @param new_file is the checked file.
*/
static LIST *list_add(LIST *list, D_FILE new_file)
{
    if(list_find(list, "", new_file.inode) != NULL)
    {
        return list;
    }
    else
    {
        statistics.nfiles += 1;
        statistics.total_size += new_file.size;

        LIST *new   = list_new_item(new_file);
        new->next   = list;
        return new;
    }
}

//  --------------------------------------------------------------------

uint32_t hash_string(char *string)
{
    uint32_t hash = 0;

    while(*string != '\0')
    {
        hash = hash*33 + *string;
        ++string;
    }
    return hash;
}

HASHTABLE *hashtable_new(void)
{
    HASHTABLE   *new = malloc(HASHTABLE_SIZE * sizeof(LIST *));
    CHECK_ALLOC(new);
    
    return new;
}

int hashtable_add(HASHTABLE *hashtable, D_FILE file)
{
    uint32_t h   = hash_string(file.hash) % HASHTABLE_SIZE;
    hashtable[h] = list_add(hashtable[h], file);

    return h;
}

LIST *hashtable_find(HASHTABLE *hashtable, char *str)
{
    uint32_t h = hash_string(str) % HASHTABLE_SIZE;
    return list_find(hashtable[h], str, -1);
}
