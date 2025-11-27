#include "funcs.h"
#include <math.h>

// f’(x) = (f(a + dx) - f(a)) / dx
float cos_derivative(float a, float dx) {
    return (cosf(a + dx) - cosf(a)) / dx;
}

int* sort(int* array, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
    return array;
}
