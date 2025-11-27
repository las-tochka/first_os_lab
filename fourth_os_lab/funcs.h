#ifndef FUNCS_H
#define FUNCS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Первая функция: производная cos(x)
float cos_derivative(float a, float dx);

// Вторая функция: сортировка массива
int* sort(int* array, size_t n);

#ifdef __cplusplus
}
#endif

#endif
