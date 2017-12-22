//
// Created by radko on 12/22/17.
//

#include <stdlib.h>
#include "keygen.h"

uint32_t get_next_key(uint32_t* k) {
    const uint32_t A0 = 0xa49ed284;
    const uint32_t A1 = 0x735203DE;

    uint32_t k1 = *k & 0x0000ffff;
    uint32_t k2 = *k >> 16;

    k1 = k1*k1;
    k2 = k2*k2;

    k1 += A0;
    k2 += A1;

    *k = ((k1 & 0x00ffff00) << 8) | ((k2 & 0x00ffff00) >> 8);

    return ((k1 & 0xff) << 24) | ((k1 & 0xff000000) >> 8) | ((k2 & 0xff) << 8) | ((k2 & 0xff000000) >> 24);
}

uint32_t neg(uint32_t a){
    uint32_t x = a & 0x7f;
    uint32_t y = -(x % 128);
    return (a & 0xFFFFFF80) | (y & 0x7f);
}

void gen_decrypt_keys(uint32_t* k, uint32_t* d, uint32_t nrounds) {
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