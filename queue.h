#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "runnable.h"

/// FIFO Queue data structure, elements are of type "runnable_t".
typedef struct queue {
    runnable_t data;
    struct queue *next;
    struct queue *prev;
} queue_t;

/// Initializes new empty queue.
void queue_init(queue_t *queue);

/// Adds new element to the queue.
int queue_add(queue_t *queue, runnable_t data);

/// Removes and returnes the first element from the queue.
runnable_t queue_get(queue_t *queue);

/// Checks whether queue is empty.
bool queue_empty(queue_t *queue);

#endif //QUEUE_H
