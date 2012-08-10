#include "chat_cache.h"


static chat_node *find_node( chat_list *cl, char *chat )
{
    cl->curr = cl->root;
    if (cl->curr != NULL){
        do {
            if (stricmp(cl->curr->name, chat) == 0)
                return cl->curr;
            cl->curr = cl->curr->next;
        } while (cl->curr != NULL);
    }
    return NULL;
}

chat_list *chat_init()
{
    chat_list *cl = malloc (sizeof(*cl));
    cl->root = cl->curr = cl->end = NULL;

    return cl;
}

void chat_add( chat_list *cl, char *chat, char *title, char *topic )
{
    chat_node *node = malloc (sizeof(*node));

    node->name  = chat;

    node->title = title;
    node->topic = topic;
    node->next  = NULL;

    /* Initalize members list */
    node->members = malloc (sizeof(node->members));
    node->members->root = node->members->end = NULL;

    /* Add chat to list */
    if (cl->root == NULL){
        node->prev = NULL;
        cl->root = cl->end = node;

    }else{
        cl->end->next = node;
        node->prev = cl->end;
        cl->end = cl->end->next;
    }
}

int chat_delete( chat_list *cl, char *chat )
{
    chat_node *prev, *next, *obnode;

    obnode = find_node(cl, chat);
    if (obnode == NULL)
        return 0;

    prev = obnode->prev;
    next = obnode->next;

    if (prev != NULL)
    {
        if (next != NULL){ // any node between first and last
            prev->next = next;
            next->prev = prev;
        }
        else { // last node in list
            prev->next = NULL;
            cl->end = prev;
        }
    }
    else {
        if (next != NULL){ // first node in list
            next->prev = NULL;
            cl->root = next;
        }
        else {
            /* This list is empty, we are doing an invalid operation.
                Can't really delete anything in an empty list.
                Try adding something first, dumbass. */
                return 0;
        }
    }
    free(obnode->members);
    free(obnode);

    return 1;
}

void print_chat_list( chat_list *cl )
{
    cl->curr = cl->root;
    if (cl->curr != NULL){
        int x = 1;
        do {
            printf("[%d|%s]->", x, cl->curr->name);
            cl->curr = cl->curr->next;
            x++;
        } while(cl->curr != NULL);
        printf("\n\n");
    }
}

/*char *chat_title( chat_list *cl, char *chat, char *nt )
{

}

char *chat_topic( chat_list *cl, char *chat, char *nt )
{

}*/

/// Member functions
/// interaction with the member list is done with these
static mem_node *chat_find_user( chat_ml *ml, char *user )
{
    ml->curr = ml->root;
    if (ml->curr != NULL){
        do {
            if (stricmp(ml->curr->user, user) == 0)
                return ml->curr;
            ml->curr = ml->curr->next;
        } while (ml->curr != NULL);
    }
    return NULL;
}

void chat_add_member( chat_list *cl, char *chat, char *user, char *priv, char *gpc )
{
    chat_ml *ml     = find_node(cl, chat)->members;
    mem_node *mem   = malloc (sizeof(*mem));

    mem->user       = user;
    mem->privclass  = priv;
    mem->gpc        = gpc;

    mem->next = NULL;

    if (ml->root != NULL){
        mem->prev = NULL;
        ml->root = ml->end = mem;
    }
    else {
        ml->end->next = mem;
        mem->prev = ml->end;
        ml->end = mem;
    }
}

int chat_remove_member( chat_list *cl, char *chat, char *user )
{
    chat_ml *ml = find_node(cl, chat)->members;
    mem_node *prev, *next, *obnode = chat_find_user(ml, user);

    if (obnode == NULL)
        return 0;

    prev = obnode->prev;
    next = obnode->next;

    if (prev != NULL)
    {
        if (next != NULL){ // any node between first and last
            prev->next = next;
            next->prev = prev;
        }
        else { // last node in list
            prev->next = NULL;
            ml->end = prev;
        }
    }
    else {
        if (next != NULL){ // first node in list
            next->prev = NULL;
            ml->root = next;
        }
        else {
            /* This list is empty, we are doing an invalid operation.
                Can't really delete anything in an empty list.
                Try adding something first, dumbass. */
                return 0;
        }
    }
    free(obnode);

    return 1;
}

void chat_free_all( chat_list *cl )
{
    cl->curr = cl->end;

    if (cl->root != NULL){
        while (cl->curr != cl->root) {

            chat_free_members(cl->curr->members); // get rid of members list

            cl->curr = cl->curr->prev; // backstep in list
            free(cl->curr->next); // free the chat from list
            cl->curr->prev = NULL;
        }
        free(cl->root);
    }
    free(cl);
}

void chat_free_members( chat_ml *ml )
{
    ml->curr = ml->end;

    if (ml->root != NULL){
        while (ml->curr != ml->root) {
            ml->curr = ml->curr->prev;
            free(ml->curr->next);
            ml->curr->prev = NULL;
        }
        free(ml->root);
    }
    free(ml);
}
