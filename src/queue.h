/********************************************************
 * Filename: queue.h
 * Author: daedalus
 * Email: 
 * Description: 
 *
 *******************************************************/
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>
#include <semaphore.h>
#include "list.h"

typedef struct queue
{
	list_t lst;
	sem_t sem;
	pthread_mutex_t mtx;
} queue_t;

int queue_init(queue_t* q);
queue_t* queue_new();
void queue_del(queue_t* q);
list_head_t* queue_dequeue(queue_t* q);
void queue_enqueue(queue_t* q, list_head_t* hdr);




#endif /* __QUEUE_H__ */

