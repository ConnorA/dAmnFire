#include "damn.h"

struct priv_vars {
    char *token;
};

/** Virtual functions make sure that the library itself has access to them. **/
static void set_status( damn *d, enum states status )
{
    d->status = status;
    d->state_change(d, status);
}

static void *async_read( void* );


/** main dAmn functions **/
damn *damn_init( char *username, char *password )
{
    damn *d = malloc (sizeof(*d));
    d->private = malloc (sizeof(d->private));

    d->sockfd = 0;
    d->recon_attempts = 0;
    d->status = DISCONNECTED;

    d->pq = pq_init();
    d->queue_lock = PTHREAD_MUTEX_INITIALIZER;

    d->username = username;
    d->private->token = get_authtoken(username, password);


    return d;
}

void damn_connect( damn *d )
{
    int sock;
    struct addrinfo addr, *res;

    #ifdef __WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        fprintf(stderr, "WSAStartup failed.\n");
    }
    #endif

    memset(&addr, 0, sizeof(addr));
    addr.ai_family = AF_INET;
    addr.ai_socktype = SOCK_STREAM;
    getaddrinfo("chat.deviantart.com", "3900", &addr, &res);

    set_status(d, CONNECTING);
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(connect(sock, res->ai_addr, res->ai_addrlen) == 0) {
        freeaddrinfo(res);

        d->sockfd = sock;
        set_status(d, CONNECTED);

    } else {
        set_status(d, DISCONNECTED);
    }
}

void damn_main( damn *d )
{
    pthread_create( &(d->recv_thread), NULL, async_read, (void*)d);
}

void damn_destroy( damn *d )
{
    pthread_kill( d->recv_thread, SIGTERM);
    close(d->sockfd);



    #ifdef __WIN32
    WSACleanup();
    #endif

    pq_free(d->pq);
    free (d->private);
    free (d);
}

damn_packet *damn_read( damn *d )
{
    pthread_mutex_lock(&(d->queue_lock));
    damn_packet *p = pq_dequeue(d->pq);
    pthread_mutex_unlock(&(d->queue_lock));

    return p;
}

/** side dAmn functions **/
void send_action( damn *d, char *chat, char *msg)
{
    char *_p = malloc (31 + strlen(chat) + strlen(msg) );
    sprintf(_p, "send chat:%s\n\naction main\n\n%s", chat, msg);

    send_raw( d, _p );

    free(_p);
}

//void send_admin( damn *d, );/// TODO
void send_ban( damn *d, char *chat, char *user)
{
    char _p[50];
    sprintf(_p, "send chat:%s\n\nban=%s\n", chat, user);

    send_raw( d, _p );
}

void send_demote( damn *d, char *chat, char *user, char *privclass)
{
    char _p[60];
    sprintf(_p, "send chat:%s\n\ndemote %s\n", chat, user);

    if (privclass != NULL)
        sprintf(_p, "\n%s\n", privclass);

    send_raw( d, _p );
}

void send_disconnect( damn *d )
{
    send_raw( d, "disconnect\n" );
    set_status(d, DISCONNECTING);
}

void send_get( damn *d, char *chat, char *property)
{
    char _p[60];
    sprintf(_p, "get chat:%s\np=%s\n", chat, property);

    send_raw( d, _p );
}

void send_whois( damn *d, char *user )
{
    char _p[50];
    sprintf(_p, "get login:%s\np=info\n", user);

    send_raw( d, _p );
}

void send_handshake( damn *d )
{
    char _p[25];
    sprintf(_p, "dAmnClient 0.3\nagent=%s\n", get_agent());

    send_raw(d, _p);
    set_status(d, HANDSHAKING);
}

void send_join( damn *d, char *chat )
{
    char _p[30];
    sprintf(_p, "join chat:%s\n", chat);
    send_raw( d, _p );
}

void send_kick( damn *d, char *chat, char *user )
{
    char _p[50];
    sprintf(_p, "kick chat:%s\n\nu=%s\n", chat, user);
    send_raw( d, _p );
}

void send_login( damn *d )
{
    char _p[70];
    sprintf(_p, "login %s\npk=%s\n", d->username, d->private->token);

    send_raw( d, _p );
}

void send_msg( damn *d, char *chat, char *to, char *msg )
{
    char *_p = malloc (27 + strlen(chat) + strlen(msg) + strlen(to));

    sprintf(_p, "send chat:%s\n\nmsg main\n\n", chat);
    if (to != NULL)
        sprintf(_p, "%s: ", to);
    strcat(_p, msg);

    send_raw( d, _p );

    free(_p);
}

void send_npmsg( damn *d, char *chat, char *to, char *msg )
{
    char *_p = malloc (29 + strlen(chat) + strlen(msg) + strlen(to));

    sprintf(_p, "send chat:%s\n\nnpmsg main\n\n", chat);
    if (to != NULL)
        sprintf(_p, "%s: ", to);
    strcat(_p, msg);

    send_raw( d, _p );

    free(_p);
}

void send_part( damn *d, char *chat )
{
    char _p[30];
    sprintf(_p, "part chat:%s\n", chat);
    send_raw( d, _p );
}

void send_ping( damn *d )
{
    send_raw( d, "ping\n" );
}

void send_pong( damn *d )
{
    send_raw( d, "pong\n" );
}

void send_promote( damn *d, char *chat, char *user, char *privclass )
{
    char _p[60];
    sprintf(_p, "send chat:%s\n\npromote %s\n", chat, user);

    if (privclass != NULL)
        sprintf(_p, "\n%s\n", privclass);


    send_raw( d, _p );
}

void send_raw( damn *d, char *raw )
{
    if (send(d->sockfd, raw, strlen(raw)+1, 0) == -1){
        perror("send");
        set_status(d, DISCONNECTED);
    }
}

void send_set( damn *d, char *chat, char *property, char *value)
{
    char *_p = malloc (strlen(chat) +
                       strlen(property) +
                       strlen(value) + 15);
    sprintf(_p, "set chat:%s\np=%s\n\n%s", chat, property, value);

    send_raw( d, _p );
}

void send_unban( damn *d, char *chat, char *user )
{
    char _p[50];
    sprintf(_p, "send chat:%s\n\nunban=%s\n", chat, user);

    send_raw( d, _p );
}


char *get_author() { return "PrairieEagle"; }

char *get_agent() { return "dAmnFire"; }

static char *raw_read( damn *d )
{
    char *buf = calloc(1, 8024);
    int num;
    if ((num = recv(d->sockfd, buf, 8023, 0)) > 0){
        buf[num] = 0;
        return buf;
    } else {
        set_status(d, DISCONNECTED);
        return NULL;
    }
}

static void *async_read( void *ptr )
{
    damn *d = (damn*) ptr;
    char *data;

    while(1)
    {
        data = raw_read(d);

        pthread_mutex_lock(&(d->queue_lock));
        pq_enqueue(d->pq, parse_packet( data ));
        pthread_mutex_unlock(&(d->queue_lock));

        free(data);
    }

    return NULL;
}
