#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

static char SERVER_PROGRAM_NAME[] = "child";

int main(int argc, char **argv) {

    // получение названия текущего !исполняемого! файла
	char progpath[1024];
	{
        // получение абсолютного пути
		ssize_t len = readlink("/proc/self/exe", progpath,
		                       sizeof(progpath) - 1);
		if (len == -1) {
			const char msg[] = "error: failed to read full program path\n";
			write(STDERR_FILENO, msg, sizeof(msg));
			exit(EXIT_FAILURE);
		}

        // сокращение до названия
		while (progpath[len] != '/')
			--len;

		progpath[len] = '\0';
	}

    // открытие каналов для связи parent - child
    int parent_to_child[2];
    if (pipe(parent_to_child) == -1) {
        const char msg[] = "error: failed to create pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    int child_to_parent[2];
	if (pipe(child_to_parent) == -1) {
		const char msg[] = "error: failed to create pipe\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		exit(EXIT_FAILURE);
	}

    // создание процесса child, поле=учение его process id
	const pid_t child = fork();

	switch (child) {
	case -1: {
		const char msg[] = "error: failed to spawn new process\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		exit(EXIT_FAILURE);
	} break;

    case 0: {

        close(parent_to_child[1]);
        close(child_to_parent[0]);

        dup2(parent_to_child[0], STDIN_FILENO);
        close(parent_to_child[0]);

        dup2(child_to_parent[1], STDOUT_FILENO);
		close(child_to_parent[1]);

		{
			char path[1024];
			snprintf(path, sizeof(path) - 1, "%s/%s", progpath, SERVER_PROGRAM_NAME);

            char *const args[] = {SERVER_PROGRAM_NAME, NULL};

			int32_t status = execv(path, args);

			if (status == -1) {
				const char msg[] = "error: failed to exec into new exectuable image\n";
				write(STDERR_FILENO, msg, sizeof(msg));
				exit(EXIT_FAILURE);
			}
		}
	} break;

    default: {

        close(parent_to_child[0]);
        close(child_to_parent[1]);

        // читаем файл и перенаправляем содержимое на pipe
        char buf[4096];
        ssize_t bytes;

        bytes = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        if (bytes <= 0) {
            const char msg[] = "error: failed to read filename from stdin\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        }
        buf[bytes] = '\0';
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';

        int fd = open(buf, O_RDONLY);
        if (fd == -1) {
            const char msg[] = "error: failed to open input file\n";
            write(STDERR_FILENO, msg, sizeof(msg) - 1);
            exit(EXIT_FAILURE);
        }

        while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
            if (write(parent_to_child[1], buf, bytes) != bytes) {
                const char msg[] = "error: failed to write to server stdin\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
                close(fd);
                exit(EXIT_FAILURE);
            }
        }
        close(fd);
        close(parent_to_child[1]);

        while ((bytes = read(child_to_parent[0], buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, bytes);
        }

		close(child_to_parent[0]);

        int status;
		wait(&status);
        if (status == -1) {
			const char msg[] = "Failed to wait for child\n";
			write(STDERR_FILENO, msg, sizeof(msg));
			exit(EXIT_FAILURE);
		}
	} break;
	}
}
