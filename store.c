#include <stdint.h>
#include "duplicates.h"

bool list_find(LIST *list, char *str, bool isHash)
{
    while(list != NULL) 
    {
        if(isHash)
        {
            if(strcmp(list->file.hash, str) == 0) return true;
        }
        else
        {
            if(strcmp(list->file.name, str) == 0) return true;
        }
        list = list->next;
    }
    return false;
}

LIST *list_new_item(D_FILE file)
{
    LIST *new = malloc( sizeof(LIST) );
    CHECK_ALLOC(new);

    new->file = file;
    new->next = NULL;

    return new;
}

LIST *list_add(LIST *list, D_FILE new_file)
{
    if(list_find(list, new_file.name, false))
    {
        return list;
    }
    else // add to head
    {
        LIST *new   = list_new_item(new_file);
        new->next   = list;
        return new;
    }
}

//  --------------------------------------------------------------------

//  FUNCTION hash_string() ACCEPTS A STRING PARAMETER,
//  AND RETURNS AN UNSIGNED 32-BIT INTEGER AS ITS RESULT

uint32_t hash_string(char *string)
{
    uint32_t hash = 0;

    while(*string != '\0') {
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

void hashtable_add(HASHTABLE *hashtable, D_FILE file)
{
    uint32_t h   = hash_string(file.hash) % HASHTABLE_SIZE;
    hashtable[h] = list_add(hashtable[h], file);
}

// str == file.hash
bool hashtable_find(HASHTABLE *hashtable, char *str)
{
    uint32_t h	= hash_string(str) % HASHTABLE_SIZE;

    return list_find(hashtable[h], str, true);
}