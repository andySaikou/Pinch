// hashmap.h

#ifndef HASHMAP_H
#define HASHMAP_H

#include "list.h"

typedef long *(*hashfun)(const void *);

struct hashmap {
    int size;
    hashfun hashfun;
    struct list buckets;
};

struct hashmap *hashmap_new(int initial_buckets, int bucket_capacity);
void hashmap_insert(struct hashmap *kvs, void *k, void *v);
void hashmap_delete(struct hashmap *kvs, void *k);
void *hashmap_lookup(struct hashmap *kvs, void *k);

#endif
