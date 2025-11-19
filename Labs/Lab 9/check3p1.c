#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 128
#define INPUT_FILE "input.txt" // Assuming the input file is named c3input.txt

/**
 * Toggles the case of alpha characters in a string using pointer arithmetic.
 *
 * @param str The string buffer to modify.
 * @return The length of the string.
 */
size_t toggle_case(char *str) {
    char *ptr = str;
    size_t length = 0;

    // Loop until the null terminator is reached
    while (*ptr != '\0') {
        if (islower(*ptr)) {
            // Convert lowercase to uppercase by subtracting 32 (ASCII difference)
            *ptr = (char)toupper(*ptr);
        } else if (isupper(*ptr)) {
            // Convert uppercase to lowercase by adding 32 (ASCII difference)
            *ptr = (char)tolower(*ptr);
        }
        ptr++;
        length++;
    }
    return length;
}

/**
 * The main program.
 */
int main(void) {
    FILE *fp;
    // Allocate the buffer on the stack. The size includes the newline and null terminator.
    char buffer[MAX_LINE_LENGTH + 2];
    pid_t pid;
    int status;
    int child_count = 0;

    // 1. Open the input file
    fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("PARENT: Error opening input file");
        return EXIT_FAILURE;
    }

    // 2. Read file line by line and create a child for each line
    while (fgets(buffer, MAX_LINE_LENGTH + 2, fp) != NULL) {
        // Remove potential newline or carriage return characters using pointer arithmetic
        char *end_ptr = buffer;
        while (*end_ptr != '\0') {
            if (*end_ptr == '\n' || *end_ptr == '\r') {
                *end_ptr = '\0';
                break;
            }
            end_ptr++;
        }

        // Fork the process
        pid = fork();

        if (pid < 0) {
            // Fork failed
            perror("PARENT: Fork failed");
            // Clean up and exit
            fclose(fp);
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // CHILD PROCESS
            size_t len;

            // Allocate a new buffer on the heap for the child to own its data,
            // preventing race conditions or data corruption if the parent reads ahead.
            // +1 for the null terminator.
            char *child_line = (char *)malloc(strlen(buffer) + 1);
            if (child_line == NULL) {
                perror("CHILD: Malloc failed");
                // The child must exit after an error, not return to the parent's loop.
                exit(EXIT_FAILURE);
            }
            // Copy the data into the child's exclusive buffer
            strcpy(child_line, buffer);

            // Toggle case and get the line length
            len = toggle_case(child_line);

            // Display result
            printf("CHILD : %s (bytes: %zu)\n", child_line, len);

            // Clean up and exit
            free(child_line);
            fclose(fp); // Close the file stream opened by the parent
            exit(EXIT_SUCCESS);
        } else {
            // PARENT PROCESS
            child_count++;
            // The parent continues to read the next line and fork a new child.
        }
    }

    // 3. Close the file stream (done by the parent)
    fclose(fp);

    // 4. Parent waits for all child processes to terminate
    // waitpid(-1, ...) waits for any child process.
    while (child_count > 0) {
        // WNOHANG would make it non-blocking, but we must block until ALL children terminate.
        // Using -1 means wait for any child process.
        pid_t terminated_pid = waitpid(-1, &status, 0);

        if (terminated_pid > 0) {
            // A child terminated
            child_count--;
        } else if (terminated_pid == -1) {
            // Error occurred (e.g., no children left to wait for, though child_count should prevent this)
            if (child_count > 0) {
                perror("PARENT: waitpid error");
            }
            break;
        }
    }

    // 5. Parent reports completion and ends
    printf("PARENT: all done\n");

    return EXIT_SUCCESS;
}