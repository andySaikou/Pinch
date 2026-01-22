#include "hashmap.h"
#include "util.h"

struct hashmap *hashmap_new(int initial_buckets, int bucket_capacity) {
    struct hashmap *result =
        xalloc(sizeof(struct hashmap), "Interpreter Error: Failed to allocate "
                                       "memory while constructing a hashmap");
    result->size = 0;
}

void hashmap_insert(struct hashmap *kvs, void *k, void *v);
void hashmap_delete(struct hashmap *kvs, void *k);
void *hashmap_lookup(struct hashmap *kvs, void *k);
