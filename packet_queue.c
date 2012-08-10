#include "packet_queue.h"

packet_queue *pq_init()
{
    packet_queue *pq = malloc(sizeof(*pq));
    pq->first = pq->last = NULL;

    return pq;
}

void pq_enqueue( packet_queue *pq, damn_packet *p )
{
    pq_node *node = malloc(sizeof(*node));
    node->packet = p;
    node->next = NULL;

    if (pq->first == NULL){
        pq->first = pq->last = node;
    }else{
        pq->last->next = node;
        pq->last = pq->last->next;
    }
}

damn_packet *pq_dequeue( packet_queue *pq )
{
    if (pq->first == NULL)
        return NULL;

    damn_packet *p = pq->first->packet;

    pq->first = pq->first->next;

    return p;
}

void pq_free( packet_queue *pq )
{
    while (pq->first != NULL)
    {
        pq_node *ob = pq->first;

        pq->first = pq->first->next;

        free_packet(ob->packet);
        free(ob);
    }
}

