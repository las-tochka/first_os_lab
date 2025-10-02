#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int is_composit(long long x) {
	if (x < 2) return 0;
	for (long long i = 2; i * i <= x; ++i) {
		if (x % i == 0) return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
    char buf[4096];
    ssize_t bytes;

    // Read numbers line by line from stdin
    char line[256];
    uint32_t linelen = 0;
    while ((bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        for (uint32_t i = 0; i < (uint32_t)bytes; ++i) {
            char c = buf[i];
            if (c == '\n') {
                line[linelen] = '\0';
                // Parse integer
                char *endptr = NULL;
                long long value = strtoll(line, &endptr, 10);
                if (endptr != line) {
                    if (!is_composit(value)) {
                        exit(EXIT_SUCCESS);
                    }
                    char out[64];
                    int32_t outlen = snprintf(out, sizeof(out), "%lld\n", value);
                    if (write(STDOUT_FILENO, out, outlen) != outlen) {
                        const char msg[] = "error: failed to echo\n";
                        write(STDERR_FILENO, msg, sizeof(msg) - 1);
                        exit(EXIT_FAILURE);
                    }
                }
                linelen = 0;
            } else {
                if (linelen + 1 < sizeof(line)) {
                    line[linelen++] = c;
                } else {
                    // Line too long, reset to avoid overflow
                    linelen = 0;
                }
            }
        }
    }

    // If the last line doesn't end with newline, process it as well
    if (linelen > 0) {
        line[linelen] = '\0';
        char *endptr = NULL;
        long long value = strtoll(line, &endptr, 10);
        if (endptr != line) {
            if (!is_composit(value)) {
                exit(EXIT_SUCCESS);
            }
            char out[64];
            int32_t outlen = snprintf(out, sizeof(out), "%lld\n", value);
            write(STDOUT_FILENO, out, outlen);
        }
    }

    if (bytes < 0) {
        const char msg[] = "error: failed to read from stdin\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        exit(EXIT_FAILURE);
    }
}