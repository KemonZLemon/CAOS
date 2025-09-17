
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: provide cache size\n");
        return EXIT_FAILURE;
    }

    int cachesize = atoi(argv[1]);
    if(cachesize <= 0) {
        fprintf(stderr, "ERROR: cache size must be positive\n");
        return EXIT_FAILURE;
    }

    int **cache = calloc(cachesize, sizeof(int *));
    int *sizes = calloc(cachesize, sizeof(int));
    if(!cache || !sizes) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    printf("Enter positive integers below (CTRL-D to end).\n");

    int value;
    while(1) {
        int ret = scanf("%d", &value);
        if(ret == EOF) {
            break;
        }
        if(ret == 0) {
            fgetc(stdin);
            continue;
        }
        int index = value % cachesize;
        int size = sizes[index];

        int hit = 0;
        int hitpos = -1;
        if(size > 0) {
            for (int i = 0; i < size; i++) {
                if(cache[index][i] == value) {
                    hit = 1;
                    hitpos = i;
                    break;
                }
            }
        }

        if(size == 0) {
            cache[index] = calloc(1, sizeof(int));
            if(!cache[index]) {
                perror("calloc");
                return EXIT_FAILURE;
            }
            cache[index][0] = value;
            sizes[index] = 1;
            printf("Value %d hashes to index %d (calloc)\n", value, index);
        } else if(hit) {
            if(hitpos ==size - 1) {
                printf("Value %d hashes to index %d (nop)\n", value, index);
            } else {
                int tmp = cache[index][hitpos];
                for(int j = hitpos; j < size -1; j++) {
                    cache[index][j] = cache[index][j+1];
                }
                cache[index][size-1] = tmp;
                printf("Value %d hashes to index %d (reorder)\n", value, index);
            }
        } else if(size < 3) {
            int * newblock = realloc(cache[index], (size +1) * sizeof(int));
            if(!newblock) {
                perror("realloc");
                return EXIT_FAILURE;
            }
            cache[index] = newblock;
            cache[index][size] = value;
            sizes[index]++;
            printf("Value %d hashes to index %d (realloc)\n", value, index);
        } else {
            for(int j = 0; j < 2; j++) {
                cache[index][j] = cache[index][j+1];
            }
            cache[index][2] = value;
            printf("Value %d hashes to index %d (shift)\n", value, index);
        }
    }
    for(int i = 0; i < cachesize; i++) {
        if(sizes[i] > 0) {
            printf("[%d] ==>", i);
            for(int j = 0; j < sizes[i]; j++) {
                printf("%d ", cache[i][j]);
                if(j < sizes[i] - 1) {
                    printf(", ");
                }
                printf("\n");
            }
        }
    }
    for(int i = 0; i < cachesize; i++) {
        free(cache[i]);
    }
    free(cache);
    free(sizes);
    return EXIT_SUCCESS;

}