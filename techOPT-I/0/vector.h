#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct vector
{
    uint8_t *data;
    size_t size;
    size_t cap;
} vector;

#define vector_init(this, n, type)                    \
    (this)->data = realloc(NULL, (n) * sizeof(type)); \
    (this)->size = 0;                               \
    (this)->cap = (n)

#define vector_push_back(this, elem, type)              \
    if ((this)->size == (this)->cap)               \
    {                                            \
        realloc((this)->data, (this)->cap *= 2); \
    }                                            \
    memcpy(((type*)((this)->data)) + (this)->size++, &(elem), sizeof(type))

#define vector_at(this, i, type) ((type*)((this)->data))[i]

#define vector_delete(this) free((type *)((this)->data))
