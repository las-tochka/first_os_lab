#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "funcs.h"

typedef float (*cos_d_t)(float, float);
typedef int*  (*sort_t)(int*, size_t);

void* lib = NULL;
cos_d_t cos_derivative_ptr = NULL;
sort_t  sort_ptr = NULL;

void print_menu() {
    printf("ДОСТУПНЫЕ КОМАНДЫ\n");
    printf("0 - Переключить реализацию (impl1 <-> impl2)\n");
    printf("1 a dx\n");
    printf("    Вычислить производную cos(x)\n");
    printf("    Пример: 1 0 0.01\n");
    printf("2 n arr...\n");
    printf("    Отсортировать массив\n");
    printf("    Пример: 2 4 9 2 3 1\n\n");
    printf("любая другая команда — выход\n");
}

void load_lib(const char* path) {
    if (lib) dlclose(lib);
    lib = dlopen(path, RTLD_LAZY);
    if (!lib) {
        printf("Ошибка загрузки библиотеки: %s\n", dlerror());
        exit(1);
    }
    cos_derivative_ptr = (cos_d_t)dlsym(lib, "cos_derivative");
    sort_ptr           = (sort_t)dlsym(lib, "sort");
    printf("Используется библиотека: %s\n", path);
}

int main() {
    print_menu();
    load_lib("./libimpl1.so");
    while (1) {
        int cmd;
        printf("Введите команду: ");
        if (scanf("%d", &cmd) != 1) {
            printf("Ошибка: ожидалось число.\n");
            scanf("%*s");
            continue;
        }
        if (cmd == 0) {
            static int toggle = 0;
            toggle = !toggle;
            if (toggle) load_lib("./libimpl2.so");
            else        load_lib("./libimpl1.so");
            printf("Библиотека переключена.\n");
        }
        else if (cmd == 1) {
            float a, dx;
            if (scanf("%f %f", &a, &dx) != 2) {
                printf("Ошибка: формат команды '1 a dx'\n");
                scanf("%*s");
                continue;
            }
            printf("Результат: cos'(a) = %f\n", cos_derivative_ptr(a, dx));
        }
        else if (cmd == 2) {
            size_t n;
            if (scanf("%zu", &n) != 1) {
                printf("Ошибка: ожидалось число n.\n");
                scanf("%*s");
                continue;
            }
            int* arr = malloc(n * sizeof(int));
            if (!arr) {
                printf("Ошибка: недостаточно памяти.\n");
                continue;
            }
            for (size_t i = 0; i < n; i++) {
                if (scanf("%d", &arr[i]) != 1) {
                    printf("Ошибка: ожидалось число массива.\n");
                    free(arr);
                    scanf("%*s");
                    continue;
                }
            }
            sort_ptr(arr, n);
            printf("Отсортированный массив: ");
            for (size_t i = 0; i < n; i++) printf("%d ", arr[i]);
            printf("\n");

            free(arr);
        }
        else {
            printf("Выход.\n");
            break;
        }
    }
    if (lib) dlclose(lib);
    return 0;
}
