#include <chacha20.h>
#include <poly1305.h>
#include <chacha20_poly1305.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define MAX_AAD_LEN 64

static void poly1305_one_time_key_gen(uint8_t key[32], uint8_t nonce[12], uint8_t res[32]) {
    uint32_t count = 0;
    uint8_t block[64];
    chacha20_block(key, count, nonce, block);
    memcpy(res, block, 32);
}
 
void chacha20_poly1305(buffer_t aad, uint8_t key[32], uint8_t iv[8], uint8_t constant[4], buffer_t m, buffer_t c, uint8_t tag[16]) {    
    uint8_t one_time_key[32];
    uint8_t nonce[12];
    memcpy(nonce, constant, 4);
    memcpy(nonce + 4, iv, 8);
    poly1305_one_time_key_gen(key, nonce, one_time_key);
    chacha20_encrypt(key, 1, nonce, m, c);
    size_t aad_size_padded = 16 * (aad.length / 16 + (aad.length % 16 != 0));
    size_t c_size_padded = 16 * (c.length / 16 + (c.length % 16 != 0));
    size_t data_len = aad_size_padded + c_size_padded + 16;
    uint8_t *data = calloc(1, data_len);
    assert(data != NULL);
    uint8_t *iter = data;
    memcpy(iter, aad.data, aad.length);
    iter += aad_size_padded;
    memcpy(iter, c.data, c.length);
    iter += c_size_padded;
    uint32_t aad_len = aad.length;
    uint32_t c_len = c.length;
    for (int j = 0; j < 4; j++) {
        iter[j] = (uint8_t) aad_len;
        aad_len >>= 8; 
    }
    iter += 8;
    for (int j = 0; j < 4; j++) {
        iter[j] = (uint8_t) c_len;
        c_len >>= 8; 
    }
    poly1305_tag(one_time_key, (buffer_t){data_len, data}, tag);
}
