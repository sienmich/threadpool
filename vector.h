#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

/// vector_t struct - a dynamically allocated array of void pointers.
typedef struct vector_t {
    int max_size; ///< Allocated memory size.
    int size; ///< vector length.
    void **data; ///< Pointer to array of elements.
} vector_t;

/// Adds new element at the end of a vector. Extends it if necessary.
int push_back(vector_t *vector, void *ptr);

/// Deletes and returns the last element from a vector.
void *pop_back(vector_t *vector);

/// Deletes an element from a vector.
void delete_element_from_vector(vector_t *vector, void *ptr);

#endif //VECTOR_H