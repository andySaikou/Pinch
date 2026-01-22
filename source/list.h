// list.h

#ifndef LIST_H
#define LIST_H

struct list {
    int capacity;
    int length;
    void **elems;
};

struct list *list_new(int initial_capacity);
void *list_get(struct list *xs, int idx);
void list_set(struct list *xs, int idx, void *x);
void list_append(struct list *xs, void *x);
void list_prepend(struct list *xs, void *x);

#endif
