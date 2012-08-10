#include "packet.h"

static size_t _parse_argpair(damn_args *p, const char *str) {
    // at the end of the argument pairs
    if (*str == 0 || *str == '\n') return 0;

    size_t idx = 0, idx_n = 0;

    // get the key
    do { idx++; } while (str[idx] != '=' && str[idx] != '\n');
    if (str[idx] == '\n') return idx + 1;

    char *key = malloc(idx + 1);
    strncpy(key, str, idx);
    key[idx] = 0;
    str += (idx + 1);

    // get the value!
    while (str[++idx_n] != '\n');

    char *value = malloc(idx_n + 1);
    strncpy(value, str, idx_n);
    value[idx_n] = 0;

    // make the argument pair
    if (p->args == NULL)
        p->args = al_make_pair(key, value);
    else
        al_set(p->args, key, value);

    // consumed key, value, '=' and '\n'
    return idx + idx_n + 2;
}

struct damn_args *parse_sub( char *data )
{
    damn_args *s = malloc (sizeof(*s));

    s->args = NULL;
    s->body = NULL;

    if (*data != '\n' && *data != 0) {
        size_t i;
        while ((i = _parse_argpair(s, data)) > 0)
            data += i;
    }

    if (*data > 0)
    {
        s->body = malloc (strlen(data)+1);
        strcpy (s->body, data+1);
        s->body[strlen(data)+1] = 0;
    }

    return s;
}

struct damn_packet *parse_packet( char *data )
{
    damn_packet *p = malloc (sizeof(*p));

    //raw
    p->raw      = malloc (strlen(data)+1);
    strcpy(p->raw, data);

    p->cmd      = NULL;
    p->param    = NULL;

    size_t fnl  = strchr(data, '\n') - data; // first new line.
    size_t fs   = strchr(data, ' ') - data; // first space that seperates command and parameter


    if (fnl != NULL) // not a valid packet without a line break in it.
    {
        if (fs != NULL && fs < fnl) // a parameter exists
        {
            p->cmd = malloc (fs);
            strncpy (p->cmd, data, fs);
            p->cmd[fs] = '\0';


            p->param = malloc ( fnl - fs);
            strncpy (p->param, data + (fs + 1), (fnl - fs) - 1);
            p->param[fnl - fs - 1] = '\0';
        }else{ // there is no parameter, but we assume a command.
            p->cmd = malloc (fnl);
            strncpy (p->cmd, data, fnl);
            p->cmd[fnl] = '\0';
        }

        data += fnl+1; // done with the first line, set pointer past it.

        if (*data == 0)
            p->sub = parse_sub( "\n\n" ); // just to initalize the sub.
        else
            p->sub = parse_sub( data );
    }

    return p;

}


void free_packet( damn_packet *p )
{
    free_args (p->sub);

    free (p->cmd);
    if (p->param != NULL) free (p->param);

    free (p->raw);

    free (p);
}

void free_args( damn_args *s )
{
    if (s->args != NULL)
        al_free (s->args);

    if (s->body != NULL) free (s->body);

    free (s);
}
