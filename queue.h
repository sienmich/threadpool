#ifndef QUEUE_H
#define QUEUE_H

#include "runnable.h"

typedef struct queue {
    runnable_t data;
    struct queue *next;
    struct queue *prev;
} queue_t;

void queue_init(queue_t *queue);

int queue_add(queue_t *queue, runnable_t data);

runnable_t queue_get(queue_t *queue);

bool queue_empty(queue_t *queue);

void queue_destroy(queue_t *queue);

#endif //QUEUE_H
