#ifndef _chat_cache_h
#define _chat_cache_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct _userinfo {
    char *privclass;    // privclass in the chat
    //char *symbol;       // user symbol
    //char *rn, *tn;    // decided to cut out unneeded information.
    char *gpc;          // Global privclass.

} chat_ui; // chatroom user information


typedef struct _mem_node {
    char *user; // search value;
    char *privclass;
    char *gpc;
    struct _mem_node *prev, *next;

} mem_node; // chatroom member list

typedef struct _mem_list {
    mem_node *root, *curr, *end;
} chat_ml;

typedef struct _chat_node {
    char *name;
    char *title;
    char *topic;

    chat_ml *members;

    struct _chat_node *prev, *next;

} chat_node;

typedef struct _chat_list {
    chat_node *root, *curr, *end;
} chat_list;

/// Main chat functions.
chat_list *chat_init();
void chat_add( chat_list *cl, char *chat, char *title, char *topic );
int chat_delete( chat_list *cl, char *chat );
//char *chat_title( chat_list *cl, char *chat, char *nt );
//char *chat_topic( chat_list *cl, char *chat, char *nt );

void print_chat_list( chat_list *cl );

/// Member functions
/// interaction with the member list is done with these
void chat_add_member( chat_list *cl, char *chat, char *user, char *priv, char *gpc );
int chat_remove_member( chat_list *cl, char *chat, char *user );

void chat_free_all( chat_list *cl );
void chat_free_members( chat_ml *ml );
#endif
