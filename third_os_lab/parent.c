#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#define SHM_SIZE 4096

const char SHM_NAME[] = "shm-lab3";
const char SEM_NAME[] = "sem-lab3";

static char SERVER_PROGRAM_NAME[] = "child";

int main() {
	int shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (shm == -1) {
		const char msg[] = "error: failed to create SHM\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	}

	if (ftruncate(shm, SHM_SIZE) == -1) {
		const char msg[] = "error: failed to resize SHM\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	}

	char *shm_buf = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm, 0);
	if (shm_buf == MAP_FAILED) {
		const char msg[] = "error: failed to map SHM\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	}
    
    sem_t *sem = sem_open(SEM_NAME, O_RDWR | O_CREAT | O_TRUNC, 0600, 1);
	if (sem == SEM_FAILED) {
		const char msg[] = "error: failed to create semaphore\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	}

	const pid_t child = fork();
    if (child == 0) {
		char *args[] = {"child", NULL};
		execv("./child", args);

		const char msg[] = "error: failed to exec\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	} else if (child == -1) {
		const char msg[] = "error: failed to fork\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		_exit(EXIT_FAILURE);
	}

    char filename[256];
    ssize_t len = read(STDIN_FILENO, filename, sizeof(filename) - 1);
    if (len <= 0) {
        perror("read filename");
        exit(EXIT_FAILURE);
    }
    filename[len] = '\0';
    char *nl = strchr(filename, '\n');
    if (nl) *nl = '\0';
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open file");
        exit(EXIT_FAILURE);
    }

    char buf[SHM_SIZE - sizeof(uint32_t)];
    ssize_t bytes;
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
		sem_wait(sem);

        uint32_t *length = (uint32_t *)shm_buf;
        char *text = shm_buf + sizeof(uint32_t);

        *length = bytes;
        memcpy(text, buf, bytes);

        sem_post(sem);
        usleep(10000);
    }

    sem_wait(sem);
    *(uint32_t *)shm_buf = UINT32_MAX;
    sem_post(sem);

    close(fd);

	waitpid(child, NULL, 0);

	sem_unlink(SEM_NAME);
	sem_close(sem);
	// забить окно в Европу наглухо
	munmap(shm_buf, SHM_SIZE);
	// отсоединить имя
	shm_unlink(SHM_NAME);
	close(shm);

    return 0;
}
