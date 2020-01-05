#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdbool.h>

/// Vector struct - a dynamically allocated array of void pointers.
typedef struct Vector {
    int maxSize; ///< Allocated memory size.
    int size; ///< Vector length.
    void **data; ///< Pointer to array of elements.
} Vector;

/// Adds new element at the end of a vector. Extends it if necessary.
int pushBack(Vector *vector, void *ptr);

/// Deletes and returns the last element from a vector.
void *popBack(Vector *vector);

/// Deletes an element from a vector.
void deleteElementFromVector(Vector *vector, void *ptr);

#endif /* __VECTOR_H__ */
