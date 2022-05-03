#ifndef CHACHA20_POLY1305_H
#define CHACHA20_POLY1305_H

#include <stdint.h>
#include <buffer.h>

void chacha20_poly1305(buffer_t aad, uint8_t key[32], uint8_t iv[8], uint8_t constant[4], buffer_t m, buffer_t c, uint8_t tag[16]);

#endif /* CHACHA20_POLY1305_H */
