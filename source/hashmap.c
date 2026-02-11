#include "hashmap.h"
#include "list.h"
#include "util.h"

static const double LOAD = 0.75;

struct hashmap *hashmap_new(int initial_buckets, int bucket_capacity,
                            hashfun hashfun) {
    struct hashmap *result =
        xalloc(sizeof(struct hashmap), "Interpreter Error: Failed to allocate "
                                       "memory while constructing a hashmap");
    result->size = 0;
    result->bucket_size = initial_buckets;
    result->hashfun = hashfun;
    struct list *buckets = list_new(initial_buckets);
    for (int i = 0; i < initial_buckets; i++) {
        list_append(buckets, list_new(bucket_capacity));
    }
    result->buckets = buckets;
    return result;
}

struct pair *pair_new(void *k, void *v) {
    struct pair *result =
        xalloc(sizeof(struct pair), "Interpreter Error: Failed to allocate "
                                    "memory while constructing a pair");
    result->k = k;
    result->v = v;
    return result;
}

static void bucket_insert(struct list *buckets, hashfun hashfun,
                          struct pair *kv) {
    int hash = hashfun(kv->k);
    struct list *bucket = list_get(buckets, hash % buckets->length);
    list_append(bucket, kv);
}

static void resize(struct hashmap *kvs) {
    kvs->bucket_size *= 2;
    struct list *new_buckets = list_new(kvs->bucket_size);
    for (int i = 0; i < kvs->buckets->length; i++) {
        struct list *bucket = list_get(kvs->buckets, i);
        for (int j = 0; j < bucket->length; j++) {
            bucket_insert(new_buckets, kvs->hashfun, list_get(bucket, j));
        }
        xfree(bucket);
    }
    xfree(kvs->buckets);
    kvs->buckets = new_buckets;
}

static void try_resize(struct hashmap *kvs) {
    if ((double)(kvs->size) / (double)(kvs->bucket_size) > LOAD) {
        resize(kvs);
    }
}

void hashmap_insert(struct hashmap *kvs, void *k, void *v) {
    try_resize(kvs);
    bucket_insert(kvs->buckets, kvs->hashfun, pair_new(k, v));
    kvs->size++;
}

static struct pair *hashmap_find(struct hashmap *kvs, void *k) {
    int hash = kvs->hashfun(k);
    struct list *bucket = list_get(kvs->buckets, hash % kvs->bucket_size);
    for (int i = 0; i < bucket->length; i++) {
        struct pair *kv = list_get(bucket, i);
        if (strcmp((char*)kv->k, (char*)k) == 0) {
            return kv;
        }
    }
    return NULL;
}

void hashmap_delete(struct hashmap *kvs, void *k) {
    struct pair *kv = hashmap_find(kvs, k);
    if (kv != NULL) {
        xfree(kv);
        kvs->size--;
    }
}

void *hashmap_lookup(struct hashmap *kvs, void *k) {
    struct pair *kv = hashmap_find(kvs, k);
    if (kv != NULL) {
        return kv->v;
    } else {
        return NULL;
    }
}
