#ifndef _packet_h
#define _packet_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "alist.h"

typedef struct damn_args damn_args;
struct damn_args {
	arglist *args;
	char *body;
};

typedef struct damn_packet damn_packet;
struct damn_packet
{
	char *cmd, *param, *raw;
	damn_args *sub;
};

struct damn_args *parse_sub( char* ); //basically parses everything below the first line of the packet.
struct damn_packet *parse_packet( char* );


void free_packet( damn_packet* );
void free_args( damn_args* );
#endif
