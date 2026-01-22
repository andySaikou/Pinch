#include "assert.h"
#include "list.h"
#include "util.h"

struct list *list_new(int initial_capacity) {
    struct list *result =
        xalloc(sizeof(struct list), "Interpreter Error: Failed to allocate "
                                    "memory while constructing a list");
    result->length = 0;
    result->capacity = initial_capacity;
    result->elems = xalloc(sizeof(void *) * initial_capacity,
                           "Interpreter Error: Failed to allocate "
                           "memory while constructing a list");
    return result;
}

void *list_get(struct list *xs, int idx) {
    assert(idx < xs->length);
    return xs->elems[idx];
}

void list_set(struct list *xs, int idx, void *x) {
    assert(idx < xs->length);
    xs->elems[idx] = x;
}

static void list_resize(struct list *xs) {
    void **buf = xalloc(xs->capacity * 2 * sizeof(void *),
                        "Interpreter Error: Failed to allocate "
                        "memory while resizing a list");
    for (int i = 0; i < xs->length; i++) {
        buf[i] = xs->elems[i];
    }
    xfree(xs->elems);
    xs->elems = buf;
    xs->capacity *= 2;
}

void list_append(struct list *xs, void *x) {
    if (xs->capacity == xs->length) {
        list_resize(xs);
    }
    xs->elems[xs->length] = x;
    xs->length++;
}

void list_prepend(struct list *xs, void *x) {
    if (xs->capacity == xs->length) {
        list_resize(xs);
    }
    for (int i = xs->length; i > 0; i--) {
        xs->elems[i] = xs->elems[i - 1];
    }
    xs->elems[0] = x;
    xs->length++;
}
