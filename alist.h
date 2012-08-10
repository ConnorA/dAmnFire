#ifndef _alist_h
#define _alist_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct _arglist {
    char *key;
    char *value;
    struct _arglist *next;
};
typedef struct _arglist arglist;

arglist *al_make_pair(char*, char*);
void al_set(arglist*, char*, char*);
char *al_get(arglist*, char*);
void al_free(arglist*);

#endif
