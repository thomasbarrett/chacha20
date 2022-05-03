#include <poly1305.h>
#include <uint.h>
#include <gfp.h>
#include <string.h>
#include <stdio.h>

#define min(a, b) (a < b ? a: b)

void poly1305_clamp(uint8_t r[32]) {
    r[3] &= 15;
    r[7] &= 15;
    r[11] &= 15;
    r[15] &= 15;
    r[4] &= 252;
    r[8] &= 252;
    r[12] &= 252;
}

void le_bytes_to_num(const uint8_t *bytes, size_t n, uint_t *res) {
    for (int i = 0; i < n; i++) {
        res[i] = (bytes[4 * i + 3] << 24) |
                 (bytes[4 * i + 2] << 16) |
                 (bytes[4 * i + 1] << 8) |
                 (bytes[4 * i]);
    }
}

void num_print(const uint_t *num, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%08x", num[n - i - 1]);
    }
    printf("\n");
}

void num_to_le_bytes(const uint_t *num, uint8_t *bytes) {
    for (int i = 0; i < 4; i++) {
        bytes[4 * i] = (uint8_t) num[i];
        bytes[4 * i + 1] = (uint8_t) (num[i] >> 8);
        bytes[4 * i + 2] = (uint8_t) (num[i] >> 16);
        bytes[4 * i + 3] = (uint8_t) (num[i] >> 24);
    }
}

void poly1305_tag(const uint8_t key[32], buffer_t m, uint8_t tag[16]) {
    uint8_t r_bytes[16];
    memcpy(r_bytes, key, 16);
    poly1305_clamp(r_bytes);
    const uint8_t *s_bytes = key + 16;

    uint32_t r[10] = {0};
    uint32_t s[5] = {0};
    le_bytes_to_num(r_bytes, 4, r);
    le_bytes_to_num(s_bytes, 4, s);

    const uint32_t p[5] = {
        0xfffffffb, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000003
    };

    uint32_t a[10] = {0};

    gfp_t gfp = gfp_init(p, 5);

    while (m.length > 0) {
        uint8_t n_bytes[20] = {0};
        memcpy(n_bytes, m.data, min(m.length, 16));
        n_bytes[min(m.length, 16)] = 1;
        uint_t n[5] = {0};
        le_bytes_to_num(n_bytes, 5, n);

        uint_add(a, n, a, 5);
        gfp_mul(&gfp, a, r, a, 5);

        m.data += 16;
        m.length -= min(m.length, 16);
    }
    uint_add(a, s, a, 5);
    num_to_le_bytes(a, tag);
}
