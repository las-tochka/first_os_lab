#include "funcs.h"
#include <math.h>

// f’(x) = (f(a + dx) - f(a - dx)) / (2dx)
float cos_derivative(float a, float dx) {
    return (cosf(a + dx) - cosf(a - dx)) / (2 * dx);
}

static void quicksort(int* arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    if (left < j) quicksort(arr, left, j);
    if (i < right) quicksort(arr, i, right);
}

int* sort(int* array, size_t n) {
    quicksort(array, 0, (int)n - 1);
    return array;
}