#ifndef CHACHA20_H
#define CHACHA20_H

#include <stdint.h>
#include <buffer.h>

void chacha20_block(uint8_t key[32], uint32_t count, uint8_t nonce[12], uint8_t out[64]);

void chacha20_encrypt(uint8_t key[32], uint32_t count, uint8_t nonce[12], buffer_t m, buffer_t c);

#endif /* CHACHA20_H */
