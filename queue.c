#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "queue.h"
#include "err.h"

void queue_init(queue_t *queue) {
    queue->next = queue;
    queue->prev = queue;
}

int queue_add(queue_t *queue, runnable_t data) {
    queue_t *new_node;
    new_node = safe_malloc(sizeof(queue_t));

    new_node->data = data;

    new_node->prev = queue;
    new_node->next = queue->next;
    queue->next->prev = new_node;
    queue->next = new_node;

    return 0;
}

runnable_t queue_get(queue_t *queue) {
    assert(!queue_empty(queue));

    queue_t *node = queue->prev;

    queue->prev = queue->prev->prev;
    queue->prev->next = queue;

    runnable_t res = node->data;
    free(node);
    return res;
}

bool queue_empty(queue_t *queue) {
    return queue->next == queue;
}

void queue_destroy(queue_t *queue) {
    while (!queue_empty(queue))
        queue_get(queue);
}