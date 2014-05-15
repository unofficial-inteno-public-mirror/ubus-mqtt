/********************************************************
 * Filename: queue.c
 * Author: daedalus
 * Email: 
 * Description: 
 *
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>

#include "queue.h"
#include "misc.h"

int queue_init(queue_t* q)
{
	list_init(&q->lst);
	sem_init(&q->sem, 0, 0);
	pthread_mutex_init(&q->mtx, NULL);
	return 0;
}

queue_t* queue_new()
{
	queue_t* q = (queue_t*) malloc(sizeof(queue_t));
	if(!q)
		return NULL;
	if(queue_init(q) < 0){
		free(q);
		return NULL;
	}
	return q;
}

void queue_del(queue_t* q)
{
	free(q);
}

list_head_t* queue_dequeue(queue_t* q)
{
	list_head_t* hdr;
	sem_wait(&q->sem);
	pthread_mutex_lock(&q->mtx);
	hdr = list_popf(&q->lst);
	pthread_mutex_unlock(&q->mtx);
	return hdr;
}

void queue_enqueue(queue_t* q, list_head_t* hdr)
{
	pthread_mutex_lock(&q->mtx);
	list_pushb(&q->lst, hdr);
	pthread_mutex_unlock(&q->mtx);
	sem_post(&q->sem);
}

