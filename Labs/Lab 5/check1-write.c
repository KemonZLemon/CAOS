#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fifo_name>\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    char *fifo_name = *(argv + 1);

    // Create FIFO if it doesn’t exist yet
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("mkfifo");
        // not fatal if it already exists
    }

    int fd = open(fifo_name, O_WRONLY);
    if (fd == -1) {
        perror("open for write failed");
        return EXIT_FAILURE;
    }

    unsigned int value = 0;
    do {
        printf("Enter positive integer (0 to exit): ");
        if (scanf("%u", &value) == 1) {
            if (value > 0) {
                int bytes_written = write(fd, &value, sizeof(unsigned int));
                if (bytes_written == -1) { perror("write"); return EXIT_FAILURE; }
                printf("Wrote %u to pipe (%d bytes)\n", value, bytes_written);
            }
        }
    } while (value != 0);

    close(fd);
    return EXIT_SUCCESS;
}
