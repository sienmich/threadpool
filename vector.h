/** @file
 * Interfejs klasy wektor - dynamicznie alokowanej tablicy
 *
 * @author Michał Siennicki <ms406340@students.mimuw.edu.pl>
 * @copyright Michał Siennicki
 * @date 28.04.2019
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdbool.h>

/** @brief Dynamicznie alokowana tablica.
* Na początku ma długość 0. Kiedy dodejemy na koniec nowy element,
* ale nie mieści się on w pamięci, alokuję dwa razy większą tablicę.
* Dzięki temu złożoność czasowa i pamięciowa wektora są liniowe.
*/
typedef struct Vector {
    int maxSize; ///< Długość zaalokowanej pamięci
    int size; ///< Długość wektora
    void **data; ///< Wskaźnik na tablicę zawierającą elementy wektora
} Vector;

/** @brief Dodaje nowy wskaźnik na koniec wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] ptr - wskaźnik, który będzie dodany na koniec
 * @return Wartość @p true, jeśli operacja się powiodła.
 * Wartość @p false, jeśli nie udało się zaalokować pamięci.
 */
int pushBack(Vector *vector, void *ptr);

/** @brief Usuwa i zwraca ostatni wskaźnik z wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @return  Ostatni wskaźnik z wektora lub NULL, gdy wektor jest pusty.
 */
void *popBack(Vector *vector);

/** @brief Zamienia miejscami dwa elementy wektora.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] a - indeks pierwszego elementu
 * @param[in] b - indeks drugiego elementu
 */
void swapElements(Vector *vector, int a, int b);

/** @brief Usuwa z wektora jeden element. Może zmienić kolejność pozostałych.
 * Przegląda po kolei elementy wektora i porównuje je z [ptr].
 * Pierwszy, który będzie równy zamienia z ostatnim i usuwa.
 * Jeśli nie znajdzie równego elementu, nic nie robi.
 * @param[in,out] vector - wskaźnik na wektor
 * @param[in] ptr - wskaźnik, który usuwa z wektora
 */
void deleteElementFromVector(Vector *vector, void *ptr);

#endif /* __VECTOR_H__ */
