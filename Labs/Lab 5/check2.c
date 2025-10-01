#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s {<fifo-name> | -f <filename>}\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    int fd;
    char *target_name;
    int to_file = 0;

    if (strcmp(*(argv + 1), "-f") == 0) {
        if (argc != 3) {
            fprintf(stderr, "USAGE: %s -f <filename>\n", *(argv + 0));
            return EXIT_FAILURE;
        }
        to_file = 1;
        target_name = *(argv + 2);
        fd = open(target_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            perror("open file failed");
            return EXIT_FAILURE;
        }
    } else {
        if (argc != 2) {
            fprintf(stderr, "USAGE: %s <fifo-name>\n", *(argv + 0));
            return EXIT_FAILURE;
        }
        target_name = *(argv + 1);

        // Create FIFO if it doesn’t exist
        if (mkfifo(target_name, 0666) == -1) {
            // not fatal if it already exists
        }

        fd = open(target_name, O_WRONLY);
        if (fd == -1) {
            perror("open fifo failed");
            return EXIT_FAILURE;
        }
    }

    unsigned int value = 0;
    do {
        printf("Enter positive integer (0 to exit): ");
        if (scanf("%u", &value) == 1) {
            if (value > 0) {
                int bytes_written = write(fd, (void *)&value, sizeof(unsigned int));
                if (bytes_written == -1) {
                    perror("write failed");
                    close(fd);
                    return EXIT_FAILURE;
                }
                if (to_file) {
                    printf("Wrote %u to \"%s\" (%d bytes)\n", value, target_name, bytes_written);
                } else {
                    printf("Wrote %u to pipe (%d bytes)\n", value, bytes_written);
                }
            }
        }
    } while (value != 0);

    close(fd);
    return EXIT_SUCCESS;
}
