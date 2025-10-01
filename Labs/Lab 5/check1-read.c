#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fifo_name>\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    char *fifo_name = *(argv + 1);

    int fd = open(fifo_name, O_RDONLY);
    if (fd == -1) {
        perror("open for read failed");
        return EXIT_FAILURE;
    }

    int bytes_read = 0;
    do {
        unsigned int value = 0;
        bytes_read = read(fd, &value, sizeof(unsigned int));
        if (bytes_read == -1) { perror("read"); return EXIT_FAILURE; }
        if (bytes_read > 0)
            printf("Read %u from pipe (%d bytes)\n", value, bytes_read);
    } while (bytes_read != 0);

    close(fd);
    return EXIT_SUCCESS;
}
