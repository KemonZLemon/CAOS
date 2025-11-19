#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 128
#define INPUT_FILE "input.txt"

// Structure to safely pass line data to each thread
typedef struct {
    // We use MAX_LINE_LENGTH + 1 for the null terminator.
    char line[MAX_LINE_LENGTH + 1];
} thread_data_t;

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
            // Convert lowercase to uppercase
            *ptr = (char)toupper(*ptr);
        } else if (isupper(*ptr)) {
            // Convert uppercase to lowercase
            *ptr = (char)tolower(*ptr);
        }
        ptr++;
        length++;
    }
    return length;
}

/**
 * Thread function to process a single line.
 * Prototype: void * thread_function_name_here(void * arg)
 */
void *line_processor(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    // Process the line in the thread's local data
    size_t len = toggle_case(data->line);

    // Output the result, using pthread_self() and printing as long unsigned int
    printf("THREAD: %lu : %s (bytes: %zu)\n",
           (long unsigned int)pthread_self(),
           data->line,
           len);

    // Free the dynamically allocated memory passed to the thread
    free(data);

    // Detached threads don't need to return a value, but we adhere to the prototype
    return NULL;
}

/**
 * The main program.
 */
int main(void) {
    FILE *fp;
    // Buffer to read the line into first. We add space for newline and null terminator.
    char buffer[MAX_LINE_LENGTH + 2];
    int thread_count = 0;

    // 1. Open the input file
    fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("MAIN: Error opening input file");
        return EXIT_FAILURE;
    }

    // 2. Read file line by line and create a thread for each line
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

        // Allocate memory for the thread data and copy the line content
        thread_data_t *data = (thread_data_t *)malloc(sizeof(thread_data_t));
        if (data == NULL) {
            perror("MAIN: Malloc failed for thread data");
            fclose(fp);
            return EXIT_FAILURE;
        }

        // Copy the line safely. We use pointer arithmetic instead of strcpy(data->line, buffer);
        {
            char *dest_ptr = data->line;
            char *src_ptr = buffer;
            while ((*dest_ptr++ = *src_ptr++) != '\0');
        }

        pthread_t thread_id;
        int err = pthread_create(&thread_id, NULL, line_processor, data);

        if (err != 0) {
            fprintf(stderr, "MAIN: Error creating thread: %d\n", err);
            free(data);
            fclose(fp);
            return EXIT_FAILURE;
        }

        // Detach the thread. The main thread will not monitor it.
        pthread_detach(thread_id);
        thread_count++;
    }

    // 3. Close the file stream
    fclose(fp);

    // 4. Sleep for 1 second to allow detached threads to complete their output
    printf("MAIN: Created %d threads. Sleeping for 1 second to wait for threads...\n", thread_count);
    sleep(1);

    // 5. Parent reports completion and ends
    printf("MAIN: all done\n");

    return EXIT_SUCCESS;
}