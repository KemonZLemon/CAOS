#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    char * name = "testfile.txt";

    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0660);

    if(fd == -1) {
        perror("open() failed");
        return EXIT_FAILURE;
    }

    printf("fd is %d\n", fd);

    int rc = write(fd, "SPARKS", 6);
    printf("Wrote %d bytes to fd %d\n", rc, fd);

    char buffer[32];
    sprintf(buffer, "ABCD%03dEFGH", fd);
    rc = write(fd, buffer, strlen(buffer));
    printf("Wrote %d bytes to fd %d\n", rc, fd);

    int important = 32768;
    rc = write(fd, &important, sizeof(int));
    printf("Wrote %d bytes to fd %d\n", rc, fd);

    short q = 0xfade;
    rc = write(fd, &q, sizeof(int));
    printf("Wrote %d bytes to fd %d\n", rc, fd);

    close(fd);
    return EXIT_SUCCESS;

}

