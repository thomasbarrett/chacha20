#ifndef POLY1305_H
#define POLY1305_H

#include <buffer.h>
#include <stdint.h>

/**
 * @brief Compute the Poly1305 tag of a message for the given one-time key.
 * @param key the one-time key.
 * @param m the message
 * @param tag the resulting tag.
 */
void poly1305_tag(const uint8_t key[32], buffer_t m, uint8_t tag[16]);

void chacha20_poly1305(const uint8_t key[32], uint8_t nonce[12], buffer_t m, buffer_t aad);

#endif /* POLY1305_H */
