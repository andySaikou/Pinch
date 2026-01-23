// hashmap.h

#ifndef HASHMAP_H
#define HASHMAP_H

#include "list.h"

typedef int (*hashfun)(const void *);

struct pair {
    void *k;
    void *v;
};

struct pair *pair_new(void *k, void *v);

struct hashmap {
    int size;
    int bucket_size;
    hashfun hashfun;
    // list<list<pair>>
    struct list *buckets;
};

struct hashmap *hashmap_new(int initial_buckets, int bucket_capacity,
                            hashfun hashfun);
void hashmap_insert(struct hashmap *kvs, void *k, void *v);
void hashmap_delete(struct hashmap *kvs, void *k);
void *hashmap_lookup(struct hashmap *kvs, void *k);

#endif
