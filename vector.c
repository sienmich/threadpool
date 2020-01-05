/** @file
 * Implementacja interfejsu klasy wektor - dynamicznie alokowanej tablicy
 *
 * @author Michał Siennicki <ms406340@students.mimuw.edu.pl>
 * @copyright Michał Siennicki
 * @date 28.04.2019
 */

#include <stdlib.h>
#include "vector.h"
#include "err.h"

/** Alokuje nową pamięć dla wektora.
 * Nie zmienia jego rozmiaru.
 *
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] newMaxSize - nowy rozmiar alokowanej pamięci
 * @return @p false gdy nie udało się zaalkować pamięci,
 * @p true w przeciwnym przypadku
 */
static int reMaxSize(Vector *vector, int newMaxSize) {
    void **tmp = realloc(vector->data, newMaxSize * sizeof(void *));
    if (tmp == NULL && newMaxSize)
        return -1;

    vector->maxSize = newMaxSize;
    vector->data = tmp;
    return 0;
}


/** Alokuje dwa razy więcej pamięci na wektor.
 * @param vector - wskaźnik na wektor
 * @return @p false gdy nie udało się zaalkować pamięci,
 * @p true w przeciwnym przypadku
 */
static int extend(Vector *vector) {
    if (vector->maxSize == 0)
        return reMaxSize(vector, 1);
    else
        return reMaxSize(vector, vector->maxSize * 2);
}

/** @brief Dodaje nowy wskaźnik na koniec wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] ptr - wskaźnik, który będzie dodany na koniec
 * @return Wartość @p true, jeśli operacja się powiodła.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
int pushBack(Vector *vector, void *ptr) {
    if (vector->size == vector->maxSize)
        easy_try(extend(vector));
    vector->data[vector->size++] = ptr;
    return 0;
}

/** @brief Usuwa i zwraca ostatni wskaźnik z wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @return  Ostatni wskaźnik z wektora lub NULL, gdy wektor jest pusty.
 */
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

/** @brief Zamienia miejscami dwa elementy wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] a - indeks pierwszego elementu
 * @param[in] b - indeks drugiego elementu
 */
void swapElements(Vector *vector, int a, int b) {
    void *tmp = vector->data[a];
    vector->data[a] = vector->data[b];
    vector->data[b] = tmp;
}

/** @brief Usuwa z wektora jeden element.
 * Może zmienić kolejność pozostałych.
 * Przegląda po kolei elementy wektora i porównuje je z [ptr].
 * Pierwszy, który będzie równy zamienia z ostatnim i usuwa.
 * Jeśli nie znajdzie równego elementu, nic nie robi.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] ptr - wskaźnik, który usuwa z wektora
 */
void deleteElementFromVector(Vector *vector, void *ptr) {
    for (int i = 0; i < vector->size; i++) {
        if (vector->data[i] == ptr) {
            swapElements(vector, i, vector->size - 1);
            popBack(vector);
            return;
        }
    }
}