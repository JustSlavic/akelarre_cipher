//
// Created by radko on 12/22/17.
//

#include <stdlib.h>
#include <memory.h>
#include "keygen.h"

void init(generator* g, uint16_t* mk) {
    memcpy(g->master_key, mk, 8*sizeof(uint16_t));

    g->__tmp[0] = g->master_key[0]*g->master_key[0];

    int i = 1, j = 1;
    while (j < 7) {
        g->__tmp[i] = g->master_key[j]*g->master_key[j];
        i++;
        g->__tmp[i] = g->master_key[j]*g->master_key[j];
        i++;
        j++;
    }

    g->__tmp[13] = g->master_key[7]*g->master_key[7];
    g->counter = 7;
}

// master key is 128 bit (array 8x16bit), keys is array 7x32bit
void build_7keys(generator* g) {
    const uint32_t A0 = 0xa49ed284;
    const uint32_t A1 = 0x735203de;

    for (int i = 0, j = 0; i < 7; ++i, j+=2) {
        uint32_t tmp_l = g->__tmp[j]   + A0;
        uint32_t tmp_r = g->__tmp[j+1] + A1;

        g->keys[i] = ((tmp_l & 0xff) << 24) |
                ((tmp_l & 0xff000000) >> 8) |
                ((tmp_r & 0xff) << 8) |
                ((tmp_r & 0xff000000) >> 24);

        g->__tmp[j] = (tmp_l & 0x00ffff00) >> 8;
        g->__tmp[j+1] = (tmp_r & 0x00ffff00) >> 8;
    }
}

uint32_t get_next_key(generator* g) {
    if (g->counter < 7) {
        return g->keys[g->counter++];
    } else {
        g->counter = 0;
        build_7keys(g);
        return g->keys[g->counter++];
    }
}

uint32_t neg(uint32_t a){
    uint32_t x = a & 0x7f;
    uint32_t y = -(x % 128);
    return (a & 0xFFFFFF80) | (y & 0x7f);
}

void gen_decrypt_keys(uint16_t* master_key, uint32_t* d, uint32_t nrounds) {
    uint32_t* k = calloc(13*nrounds + 9, sizeof(uint32_t));
    gen_encrypt_keys(master_key, k, nrounds);

    d[0] = -k[4 + 13*nrounds + 1];
    d[1] =  k[4 + 13*nrounds + 2];
    d[2] =  k[4 + 13*nrounds + 3];
    d[3] = -k[4 + 13*nrounds + 4];

    for (int r = 0; r < nrounds; ++r) {
        d[13*r+4] = neg(k[4 + 13*(nrounds-r)]);
        for(int j = 5; j <= 16; ++j){
            d[13*r+j] = k[13*(nrounds - r - 1) + j];
        }
    }

    d[13*nrounds + 4] = neg(k[4]);

    d[13*nrounds + 5] = -k[0];
    d[13*nrounds + 6] =  k[1];
    d[13*nrounds + 7] =  k[2];
    d[13*nrounds + 8] = -k[3];
}

void gen_encrypt_keys(uint16_t* master_key, uint32_t* k, uint32_t r) {
    generator g;
    init(&g, master_key);

    for (int i = 0; i < 4; ++i) {
        k[i] = get_next_key(&g);
    }
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < 13; ++j) {
            k[4 + i*r + j] = get_next_key(&g);
        }
    }
    for (int i = 0; i < 5; ++i) {
        k[4 + 13*r + i] = get_next_key(&g);
    }
}
