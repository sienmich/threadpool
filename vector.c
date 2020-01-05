#include <stdlib.h>
#include "vector.h"
#include "err.h"

/// Allocates new memory for a vector.
static int reMaxSize(Vector *vector, int newMaxSize) {
    void **tmp = realloc(vector->data, newMaxSize * sizeof(void *));
    if (tmp == NULL && newMaxSize)
        return -1;

    vector->maxSize = newMaxSize;
    vector->data = tmp;
    return 0;
}

/// Allocates twice the previous size for new elements.
static int extend(Vector *vector) {
    if (vector->maxSize == 0)
        return reMaxSize(vector, 1);
    else
        return reMaxSize(vector, vector->maxSize * 2);
}

/// Adds new element at the end of a vector. Extends it if necessary.
int pushBack(Vector *vector, void *ptr) {
    if (vector->size == vector->maxSize)
        easy_try(extend(vector));
    vector->data[vector->size++] = ptr;
    return 0;
}

/// Deletes and returns the last element from a vector.
void *popBack(Vector *vector) {
    if (!vector->size)
        return NULL;
    vector->size--;
    void *res = vector->data[vector->size];
    if (vector->size == 0) {
        free(vector->data);
        vector->data = NULL;
        vector->maxSize = 0;
    }
    return res;
}

/// Swaps two elements from a vector.
static void swapElements(Vector *vector, int a, int b) {
    void *tmp = vector->data[a];
    vector->data[a] = vector->data[b];
    vector->data[b] = tmp;
}

/// Deletes an element from a vector.
void deleteElementFromVector(Vector *vector, void *ptr) {
    for (int i = 0; i < vector->size; i++) {
        if (vector->data[i] == ptr) {
            swapElements(vector, i, vector->size - 1);
            popBack(vector);
            return;
        }
    }
}