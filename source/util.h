// util.h

#ifndef UTIL_H
#define UTIL_H

#include "stddef.h"

void *xalloc(size_t size, char *msg);
void xfree(void *mem);

#endif
