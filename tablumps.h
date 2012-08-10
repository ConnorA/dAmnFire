#ifndef _tablumps_h
#define _tablumps_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct _lump {
    char *find;
    char *repl;
    unsigned char arity;
    unsigned char groups[3];
};

typedef struct _lump lump;

char *delump(char *);


#endif
