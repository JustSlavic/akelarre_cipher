//
// Created by radko on 12/22/17.
//

#ifndef AKELARRE_KEYGEN_H
#define AKELARRE_KEYGEN_H

#include <stdint.h>

typedef struct __generator{
    uint16_t master_key[8];
    uint32_t __tmp[14];
    uint32_t keys[7];

    int counter; // when counter hits 7 generator have to generate another 7 values and set counter to 0
} generator;

void init(generator* g, uint16_t* mk);

uint32_t get_next_key(generator* g);

void gen_encrypt_keys(uint16_t* master_key, uint32_t* k, uint32_t nrounds);

void gen_decrypt_keys(uint16_t* master_key, uint32_t* d, uint32_t nrounds);

#endif //AKELARRE_KEYGEN_H
