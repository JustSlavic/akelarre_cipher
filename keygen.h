//
// Created by radko on 12/22/17.
//

#ifndef AKELARRE_KEYGEN_H
#define AKELARRE_KEYGEN_H

#include <stdint.h>
#include <stddef.h>

uint32_t get_next_key(uint32_t* k);

void gen_decrypt_keys(uint32_t* k, uint32_t* d, uint32_t nrounds);

#endif //AKELARRE_KEYGEN_H
