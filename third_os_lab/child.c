#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <limits.h>

#define SHM_SIZE 4096

const char SHM_NAME[] = "shm-lab3";
const char SEM_NAME[] = "sem-lab3";

static bool is_composite(long long x) {
    if (x < 2) return false;
    for (long long i = 2; i * i <= x; ++i) {
        if (x % i == 0) return true;
    }
    return false;
}

int main(void) {
    // Открываем shared memory
    int shm = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm == -1) {
        perror("shm_open");
        _exit(EXIT_FAILURE);
    }

    char *shm_buf = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (shm_buf == MAP_FAILED) {
        perror("mmap");
        _exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(SEM_NAME, O_RDWR);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        _exit(EXIT_FAILURE);
    }

    char line[256];
    uint32_t linelen = 0;
    bool running = true;

    while (running) {
        sem_wait(sem);

        uint32_t *length = (uint32_t *)shm_buf;
        char *text = shm_buf + sizeof(uint32_t);

        uint32_t bytes = *length;

        if (bytes == UINT32_MAX) {
            sem_post(sem);
            break; // сигнал конца данных
        }

        for (uint32_t i = 0; i < bytes; ++i) {
            char c = text[i];
            if (c == '\n') {
                line[linelen] = '\0';
                char *endptr = NULL;
                long long value = strtoll(line, &endptr, 10);

                if (endptr != line) {
                    if (!is_composite(value)) {
                        // Освобождаем ресурсы перед выходом
                        sem_post(sem);
                        sem_close(sem);
                        munmap(shm_buf, SHM_SIZE);
                        close(shm);
                        _exit(EXIT_SUCCESS);
                    }

                    char out[64];
                    int32_t outlen = snprintf(out, sizeof(out), "%lld\n", value);
                    if (write(STDOUT_FILENO, out, outlen) != outlen) {
                        perror("write");
                        sem_post(sem);
                        sem_close(sem);
                        munmap(shm_buf, SHM_SIZE);
                        close(shm);
                        _exit(EXIT_FAILURE);
                    }
                }
                linelen = 0;
            } else {
                if (linelen + 1 < sizeof(line)) {
                    line[linelen++] = c;
                } else {
                    linelen = 0; // переполнение строки, сбрасываем
                }
            }
        }

        sem_post(sem);
        usleep(10000); // немного подождём, чтобы не захватывать семафор слишком быстро
    }

    // Обработка последней незавершённой строки (если есть)
    if (linelen > 0) {
        line[linelen] = '\0';
        char *endptr = NULL;
        long long value = strtoll(line, &endptr, 10);
        if (endptr != line) {
            if (!is_composite(value)) {
                sem_close(sem);
                munmap(shm_buf, SHM_SIZE);
                close(shm);
                _exit(EXIT_SUCCESS);
            }
            char out[64];
            int32_t outlen = snprintf(out, sizeof(out), "%lld\n", value);
            write(STDOUT_FILENO, out, outlen);
        }
    }

    // Очистка
    sem_close(sem);
    munmap(shm_buf, SHM_SIZE);
    close(shm);

    return 0;
}
