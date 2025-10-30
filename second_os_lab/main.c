#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "functions.c"

int *array;
int n;
int num_threads;
pthread_barrier_t barrier;

void odd_even_sort_seq(int *arr, int n) {
    int sorted = 0;
    while (!sorted) {
        sorted = 1;
        // Четная фаза
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                sorted = 0;
            }
        }
        // Нечетная фаза
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                sorted = 0;
            }
        }
    }
}

pthread_barrier_t barrier;

// структура потока
typedef struct {
    int id;
} thread_data_t;

void* thread_func(void* arg) {
    thread_data_t *data = (thread_data_t*)arg;
    int id = data->id;

    int phase, i, start;
    for (phase = 0; phase < n; phase++) {
        start = (phase % 2 == 0) ? 0 : 1;

        for (i = start + id * 2; i < n - 1; i += num_threads * 2) {
            if (array[i] > array[i + 1])
                swap(&array[i], &array[i + 1]);
        }
    }
    pthread_barrier_wait(&barrier);
    return NULL;
}

void odd_even_sort_parallel(int *arr, int n, int num_threads) {
    pthread_t threads[num_threads];
    thread_data_t data[num_threads];

    pthread_barrier_init(&barrier, NULL, num_threads);

    for (int i = 0; i < num_threads; i++) {
        data[i].id = i;
        pthread_create(&threads[i], NULL, thread_func, &data[i]);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(&barrier);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("You had to enter array_size and num_threads\n");
        return 1;
    }

    char *endptr1, *endptr2;
    n = (int)strtol(argv[1], &endptr1, 10);
    num_threads = (int)strtol(argv[2], &endptr2, 10);
    if (*endptr1 != '\0' || *endptr2 != '\0' || num_threads <= 0 || n <= 0) {
        printf("Invalid input data: %s %s\n", argv[1], argv[2]);
        return 1;
    }

    array = malloc(sizeof(int) * n);

    // printf("Enter %d integers:\n", n);
    // for (int i = 0; i < n; i++) {
    //     scanf("%d", &array[i]);
    // }
    printf("Generating array of size %d...\n", n);
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 10000;
    }

    double start_time, end_time, elapsed_time;

    if (num_threads == 1) {
        start_time = get_time_ms();
        odd_even_sort_seq(array, n);
        end_time = get_time_ms();
        elapsed_time = end_time - start_time;

        printf("Linear time: %.3f\n", elapsed_time);
        // printArray(array, n);
    } else {
        start_time = get_time_ms();
        odd_even_sort_parallel(array, n, num_threads);
        end_time = get_time_ms();
        elapsed_time = end_time - start_time;

        printf("Paralel time (%d threads): %.3f\n", num_threads, elapsed_time);
        // printArray(array, n);

        // Проверка корректности
        char mistake[256] = "";
        int result = check_sort_correctness(array, n, mistake);
        if (!result) {
            printf("%s\n", mistake);
        }
    }
    free(array);
    return 0;
}
