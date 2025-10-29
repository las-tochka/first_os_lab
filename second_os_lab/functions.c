#include <sys/time.h>
#include <stdio.h>
#include <string.h>

double get_time_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int check_sort_correctness(int *a, int n, char mistake[]) {
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            snprintf(mistake, 256, "Wrong sort for index: %d", i);
            return 1;
        }
    }
    return 0;
}

void printArray(int *a, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}