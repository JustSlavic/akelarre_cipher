//
// Created by radko on 12/22/17.
//

#include "akelarre.h"

uint32_t rotate31l(uint32_t w, uint32_t shift) {
    // rotate 31 bit to left, don't touch 1 bit to the left
    // [10000000 00000000 00000000 00000000]

    // in case shift > 31, shift (mod 31)
    shift %= 31;

    uint32_t bit = w * 0x80000000;
    w &= 0x7fffffff;

    return (((w << shift) | (w >> (31-shift))) & (0x7fffffff)) | bit;
}

uint32_t rotate31r(uint32_t w, uint32_t shift) {
    // rotate 31 bit to left, don't touch 1 bit to the right
    // [00000000 00000000 00000000 00000001]

    // in case shift > 31, shift (mod 31)
    shift %= 31;

    uint32_t bit = 0x1 & w; // cut most right bit [0..000X]
    w &= 0xfffffffe;

    return (((w << shift) | (w >> (31 - shift))) & (0xfffffffe)) | bit;
}

void rotate128(Word* w, uint32_t shift) {
    shift %= 128;
    if (shift == 0) return;
    if (shift == 64) {
        uint64_t tmp = w->big_chunk[0];
        w->big_chunk[0] = w->big_chunk[1];
        w->big_chunk[1] = tmp;
    } else if (shift < 64) {
        uint64_t tmp0 = w->big_chunk[0];
        uint64_t tmp1 = w->big_chunk[1];

        w->big_chunk[0] = (tmp1 >> (64 - shift)) | (tmp0 << shift);
        w->big_chunk[1] = (tmp0 >> (64 - shift)) | (tmp1 << shift);
    } else if (shift > 64) {
        uint64_t tmp = w->big_chunk[0];
        w->big_chunk[0] = w->big_chunk[1];
        w->big_chunk[1] = tmp;

        shift %= 64;
        uint64_t tmp0 = w->big_chunk[0];
        uint64_t tmp1 = w->big_chunk[1];

        w->big_chunk[0] = (tmp1 >> (64 - shift)) | (tmp0 << shift);
        w->big_chunk[1] = (tmp0 >> (64 - shift)) | (tmp1 << shift);
    }
}


void input_transformation(Word* w, uint32_t* k) {
    w->chunk[0] += k[0];
    w->chunk[1] ^= k[1];
    w->chunk[2] ^= k[2];
    w->chunk[3] += k[3];
}

// assume r in [1..rounds]
void iterate(Word* w, const unsigned r, uint32_t* k) {
    // cyclic rotation of the 128 bit block
    rotate128(w, k[4 + 13*(r-1)] & 0x7f);

    uint32_t p1 = w->chunk[0] ^ w->chunk[2];
    uint32_t p2 = w->chunk[1] ^ w->chunk[3];

    // shifts is taken from parts of p1:
    //
    // [00000000 00000000 00000000 00011111]
    // [00000000 00000000 00000011 11100000]
    // [00000000 00000000 01111100 00000000]
    // [00000000 00001111 10000000 00000000]
    // [00000000 11110000 00000000 00000000]
    // [00001111 00000000 00000000 00000000]
    // [11110000 00000000 00000000 00000000]

    p2 = rotate31r(p2, p1 & 0x1f);
    p2 += k[4 + 13*(r-1) + 1]; // += Z_2

    p2 = rotate31l(p2, (p1 >> 5) & 0x1f);
    p2 += k[4 + 13*(r-1) + 2]; // += Z_3

    p2 = rotate31r(p2, (p1 >> 10) & 0x1f);
    p2 += k[4 + 13*(r-1) + 3]; // += Z_4

    p2 = rotate31l(p2, (p1 >> 15) & 0x1f);
    p2 += k[4 + 13*(r-1) + 4]; // += Z_5

    p2 = rotate31r(p2, (p1 >> 20) & 0xf);
    p2 += k[4 + 13*(r-1) + 5]; // += Z_6

    p2 = rotate31l(p2, (p1 >> 24) & 0xf);
    p2 += k[4 + 13*(r-1) + 6]; // += Z_7

    p2 = rotate31r(p2, (p1 >> 28) & 0xf);

    // making p1 chain

    p1 = rotate31r(p1, p2 & 0x1f);
    p1 += k[4 + 13*(r-1) + 7]; // += Z_8

    p1 = rotate31l(p1, (p2 >> 5) & 0x1f);
    p1 += k[4 + 13*(r-1) + 8]; // += Z_9

    p1 = rotate31r(p1, (p2 >> 10) & 0x1f);
    p1 += k[4 + 13*(r-1) + 9]; // += Z_10

    p1 = rotate31l(p1, (p2 >> 15) & 0x1f);
    p1 += k[4 + 13*(r-1) + 10]; // += Z_11

    p1 = rotate31r(p1, (p2 >> 20) & 0xf);
    p1 += k[4 + 13*(r-1) + 11]; // += Z_12

    p1 = rotate31l(p1, (p2 >> 24) & 0xf);
    p1 += k[4 + 13*(r-1) + 12]; // += Z_13

    p1 = rotate31r(p1, (p2 >> 28) & 0xf);

    w->chunk[0] ^= p2;
    w->chunk[1] ^= p1;
    w->chunk[2] ^= p2;
    w->chunk[3] ^= p1;
}

void output_transformation(Word* w, uint32_t* k, uint32_t nrounds) {
    rotate128(w, k[4 + 13*nrounds] & 0x7f);

    w->chunk[0] += k[4 + 13*nrounds + 1];
    w->chunk[1] ^= k[4 + 13*nrounds + 2];
    w->chunk[2] ^= k[4 + 13*nrounds + 3];
    w->chunk[3] += k[4 + 13*nrounds + 4];
}


