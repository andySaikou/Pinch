// hashmap.h

#ifndef HASHMAP_H
#define HASHMAP_H

#include "list.h"

struct hashmap {
    int capacity;
    int size;
    struct list buckets;
};

#endif
