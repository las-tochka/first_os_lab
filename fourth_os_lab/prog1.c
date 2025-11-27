#include <stdio.h>
#include <stdlib.h>
#include "funcs.h"

// export LD_LIBRARY_PATH=. если не сработает
void print_menu() {
    printf("ДОСТУПНЫЕ КОМАНДЫ\n");
    printf("1 a dx\n");
    printf("    Вычислить производную cos(x) в точке a с шагом dx\n");
    printf("    Пример: 1 3.14 0.001\n");
    printf("2 n arr...\n");
    printf("    Отсортировать массив из n целых чисел\n");
    printf("    Пример: 2 5 3 9 1 6 2\n\n");
    printf("любая другая команда — выход\n");
}

int main() {
    print_menu();

    while (1) {
        int command;
        printf("Введите команду: ");

        if (scanf("%d", &command) != 1) {
            printf("Ошибка: ожидалось число команды.\n");
            scanf("%*s");
            continue;
        }
        if (command == 1) {
            float a, dx;
            if (scanf("%f %f", &a, &dx) != 2) {
                printf("Ошибка: формат команды '1 a dx'\n");
                scanf("%*s");
                continue;
            }
            printf("Результат: cos'(a) = %f\n", cos_derivative(a, dx));
        }
        else if (command == 2) {
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
                    printf("Ошибка: ожидалось целое число массива.\n");
                    free(arr);
                    scanf("%*s");
                    continue;
                }
            }
            sort(arr, n);
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
    return 0;
}
