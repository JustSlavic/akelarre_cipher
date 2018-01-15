//
// Created by radko on 12/22/17.
//

#ifndef AKELARRE_AKELARRE_H
#define AKELARRE_AKELARRE_H

#include <stdint.h>

typedef union {
    char word[16];
    uint32_t chunk[4];
    uint64_t big_chunk[2];
} Word, *PWord;

void input_transformation(Word* w, const uint32_t* k);
void iterate(Word* w, unsigned r, const uint32_t* k);
void output_transformation(Word* w, const uint32_t* k, uint32_t rounds);

void rotate128(Word* w, uint32_t shift);
uint32_t rotate31l(uint32_t w, uint32_t shift);
uint32_t rotate31r(uint32_t w, uint32_t shift);


#endif //AKELARRE_AKELARRE_H
