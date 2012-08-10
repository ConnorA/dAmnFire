#ifndef _auth_h
#define _auth_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/typecheck-gcc.h>

char *get_authtoken (char*, char*);

#endif
