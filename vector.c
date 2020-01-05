#include <stdlib.h>
#include "vector.h"
#include "err.h"

/// Allocates new memory for a vector.
static int re_max_size(vector_t *vector, int new_max_size) {
    void **tmp = realloc(vector->data, new_max_size * sizeof(void *));
    if (tmp == NULL && new_max_size)
        return -1;

    vector->max_size = new_max_size;
    vector->data = tmp;
    return 0;
}

/// Allocates twice the previous size for new elements.
static int extend(vector_t *vector) {
    if (vector->max_size == 0)
        return re_max_size(vector, 1);
    else
        return re_max_size(vector, vector->max_size * 2);
}

/// Adds new element at the end of a vector. Extends it if necessary.
int push_back(vector_t *vector, void *ptr) {
    if (vector->size == vector->max_size)
        easy_try(extend(vector));
    vector->data[vector->size++] = ptr;
    return 0;
}

/// Deletes and returns the last element from a vector.
void *pop_back(vector_t *vector) {
    if (!vector->size)
        return NULL;
    vector->size--;
    void *res = vector->data[vector->size];
    if (vector->size == 0) {
        free(vector->data);
        vector->data = NULL;
        vector->max_size = 0;
    }
    return res;
}

/// Swaps two elements from a vector.
static void swap(vector_t *vector, int a, int b) {
    void *tmp = vector->data[a];
    vector->data[a] = vector->data[b];
    vector->data[b] = tmp;
}

/// Deletes an element from a vector.
void delete_element_from_vector(vector_t *vector, void *ptr) {
    for (int i = 0; i < vector->size; i++) {
        if (vector->data[i] == ptr) {
            swap(vector, i, vector->size - 1);
            pop_back(vector);
            return;
        }
    }
}