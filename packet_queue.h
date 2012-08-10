#ifndef _packet_queue_h
#define _packet_queue_h

#include <stdlib.h>

#include "packet.h"

typedef struct _pq_node {
    damn_packet *packet;
    struct _pq_node *next;
} pq_node;

typedef struct _pq {
    pq_node *first, *last;
} packet_queue;


packet_queue *pq_init();

void pq_enqueue( packet_queue *pq, damn_packet *p );
damn_packet *pq_dequeue( packet_queue *pq );

void pq_free( packet_queue *pq );

#endif
