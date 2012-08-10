#ifndef _damn_h
#define _damn_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#ifdef __WIN32
    #define WIN32_LEAN_AND_MEAN

    #ifdef WINVER
    #undef WINVER
    #endif

    #define WINVER 0x501

    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#include "packet.h"
#include "auth.h"
#include "packet_queue.h"

/// Connection states. Technically the current 'status' of the connection.
enum states {
    DISCONNECTED,
    DISCONNECTING,
    CONNECTING,
    CONNECTED,
    HANDSHAKING,
    LOGGEDIN
};

struct priv_vars; // variables not to be changed once set.

typedef struct damn damn;
struct damn
{
    int sockfd;
    int status; // the current status of the socket.
    int recon_attempts; // how many times we have tried to reconnect after disconnecting.

    pthread_t recv_thread;
    pthread_mutex_t queue_lock;

    char *username;
    struct priv_vars *private;

    void (*state_change)(struct damn*, enum states);

    packet_queue *pq;


};

/** main dAmn functions **/
damn *damn_init( char*, char* );
void damn_connect( damn* );
void damn_destroy( damn* );

void damn_main( damn* );
damn_packet *damn_read( damn* );

//void set_status( damn *d, enum states status );
//void pkt_handler( damn *d, damn_packet *p );
//void print_out( damn *d, enum msg_type type, ... );

/** side dAmn functions **/
void send_action( damn *d, char *chat, char *msg);
void send_ban( damn *d, char *chat, char *user);
void send_demote( damn *d, char *chat, char *user, char *privclass);
void send_disconnect( damn *d );
void send_get( damn *d, char *chat, char *property);
void send_whois( damn *d, char *user );
void send_handshake( damn *d );
void send_join( damn *d, char *chat );
void send_kick( damn *d, char *chat, char *user );
void send_login( damn *d );
void send_msg( damn *d, char *chat, char *to, char *msg );
void send_npmsg( damn *d, char *chat, char *to, char *msg );
void send_part( damn *d, char *chat );
void send_ping( damn *d );
void send_pong( damn *d );
void send_promote( damn *d, char *chat, char *user, char *privclass );
void send_raw( damn *d, char *raw );
void send_set( damn *d, char *chat, char *property, char *value);
void send_unban( damn *d, char *chat, char *user );

char *get_author();
char *get_agent();



#endif
