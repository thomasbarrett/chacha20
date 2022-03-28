#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct buffer {
    size_t length;
    uint8_t *data;
} buffer_t;

#endif /* BUFFER_H */
