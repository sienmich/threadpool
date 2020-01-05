#include <stdlib.h>
#include "queue.h"
#include "err.h"

/// Initializes new empty queue.
void queue_init(queue_t *queue) {
    queue->next = queue;
    queue->prev = queue;
}

/// Adds new element to the queue.
int queue_add(queue_t *queue, runnable_t data) {
    queue_t *new_node;
    try_ptr(new_node = malloc(sizeof(queue_t)));

    new_node->data = data;

    new_node->prev = queue;
    new_node->next = queue->next;
    queue->next->prev = new_node;
    queue->next = new_node;

    return 0;
}

/// Removes and returnes the first element from the queue.
runnable_t queue_get(queue_t *queue) {
    queue_t *node = queue->prev;

    queue->prev = queue->prev->prev;
    queue->prev->next = queue;

    runnable_t res = node->data;
    free(node);
    return res;
}

/// Checks whether queue is empty.
bool queue_empty(queue_t *queue) {
    return queue->next == queue;
}