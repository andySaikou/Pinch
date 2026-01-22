#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void *xalloc(size_t size, char *msg) {
    void *mem = malloc(size);
    if (mem == NULL) {
        fprintf(stderr, "%s", msg);
        exit(EXIT_FAILURE);
    }
    return mem;
}

void xfree(void *mem) {
    assert(mem != NULL);
    free(mem);
}
