#include <stdint.h>
#include <string.h>
#include <chacha20.h>
#include <stdio.h>

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

#define QR(a, b, c, d) (			    \
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7))

static uint32_t u32le(uint8_t *bytes) {
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

void chacha20_block(uint8_t key[32], uint32_t count, uint8_t nonce[12], uint8_t out[64]) {
    uint32_t x0[16] = {
        0x61707865,      0x3320646e,       0x79622d32,       0x6b206574,
        u32le(&key[0]),  u32le(&key[4]),   u32le(&key[8]),   u32le(&key[12]),
        u32le(&key[16]), u32le(&key[20]),  u32le(&key[24]),  u32le(&key[28]),
        count,           u32le(&nonce[0]), u32le(&nonce[4]), u32le(&nonce[8])
    };
    uint32_t x[16];
    memcpy(x, x0, sizeof(x0));
    for (int i = 0; i < 10; i++) {
        QR(x[0], x[4], x[8],  x[12]); // column 0
		QR(x[1], x[5], x[9],  x[13]); // column 1
		QR(x[2], x[6], x[10], x[14]); // column 2
		QR(x[3], x[7], x[11], x[15]); // column 3
		QR(x[0], x[5], x[10], x[15]); // diagonal 1
		QR(x[1], x[6], x[11], x[12]); // diagonal 2
		QR(x[2], x[7], x[8],  x[13]); // diagonal 3
		QR(x[3], x[4], x[9],  x[14]); // diagonal 4     
    }
    for (int i = 0; i < 16; i++) {
        uint32_t tmp = x[i] + x0[i];
        for (int j = 0; j < 4; j++) {
            out[4 * i + j] = (uint8_t) tmp;
            tmp >>= 8; 
        }
    }
}

void chacha20_encrypt(uint8_t key[32], uint32_t count, uint8_t nonce[12], buffer_t m, buffer_t c) {
    uint8_t key_stream[64];
    for (size_t i = 0; i < m.length / 64; i++) {
        chacha20_block(key, count + i, nonce, key_stream);
        for (size_t j = 0; j < 64; j++) {
            c.data[64 * i + j] = m.data[64 * i + j] ^ key_stream[j];
        }
    }
    if (m.length % 64 != 0) {
        size_t i = m.length / 64;
        chacha20_block(key, count + i, nonce, key_stream);
        for (size_t j = 0; j < m.length % 64; j++) {
            c.data[64 * i + j] = m.data[64 * i + j] ^ key_stream[j];
        }
    }
}
