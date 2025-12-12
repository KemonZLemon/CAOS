#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

unsigned int bytes = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * handlefile(void * arg);

int main(int argc, char ** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char * dirpath = *(argv + 1);

    DIR * dir = opendir(dirpath);
    if (dir == NULL) { perror("opendir() failed"); return EXIT_FAILURE; }

    struct dirent * entry;
    size_t count = 0;

    while ((entry = readdir(dir)) != NULL)
        count++;

    rewinddir(dir);

    pthread_t * tids = malloc(sizeof(pthread_t) * count);
    if (tids == NULL) { perror("malloc"); return EXIT_FAILURE; }

    size_t idx = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        printf("MAIN: assigning file \"%s\" to new child thread...\n",
               entry->d_name);

        size_t dlen = strlen(dirpath);
        size_t flen = strlen(entry->d_name);

        char * fullpath = malloc(dlen + 1 + flen + 1);
        if (fullpath == NULL) { perror("malloc fullpath"); return EXIT_FAILURE; }

        char * dst = fullpath;
        char * src = dirpath;
        while (*src != '\0') { *dst = *src; dst++; src++; }

        *dst = '/'; dst++;

        src = entry->d_name;
        while (*src != '\0') { *dst = *src; dst++; src++; }

        *dst = '\0';

        int rc = pthread_create(tids + idx, NULL, handlefile, fullpath);
        if (rc != 0) {
            fprintf(stderr, "pthread_create() failed\n");
            return EXIT_FAILURE;
        }
        idx++;
    }

    for (size_t i = 0; i < idx; i++)
        pthread_join(*(tids + i), NULL);

    closedir(dir);
    free(tids);

    printf("MAIN: total bytes across all files: %u\n", bytes);

    return EXIT_SUCCESS;
}

void * handlefile(void * arg)
{
    char * fname = arg;

    struct stat fileinfo;
    int rc = lstat(fname, &fileinfo);
    if (rc == -1) { perror("THREAD: lstat() failed"); free(fname); return NULL; }

    printf("THREAD: file \"%s\" (%ld bytes)", fname, fileinfo.st_size);

    if (S_ISREG(fileinfo.st_mode)) printf(" -- regular file");
    else if (S_ISDIR(fileinfo.st_mode)) printf(" -- directory");
    else if (S_ISLNK(fileinfo.st_mode)) printf(" -- symlink");
    else if (S_ISFIFO(fileinfo.st_mode)) printf(" -- fifo");
    else printf(" -- unknown?");

    printf(" -- last modified %s", ctime(&fileinfo.st_mtime));

    pthread_mutex_lock(&lock);
    bytes += fileinfo.st_size;
    pthread_mutex_unlock(&lock);

    free(fname);
    return NULL;
}
