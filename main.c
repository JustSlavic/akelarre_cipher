#include <stdio.h>
#include <stdlib.h>
#include "akelarre.h"
#include "keygen.h"

#define input_file "message.txt"
#define output_file "cipher"
#define word_size 16
#define rounds 1

int main() {
    FILE* inFile = fopen(input_file, "rb");
    if (!inFile) {
        printf("bad file\n");
        return 1;
    }
    char* buffer = calloc(word_size, sizeof(char));
    fread(buffer, sizeof(char), word_size, inFile);
    fclose(inFile);
    PWord word = (PWord) buffer;

    // ======== generate key schedule ========
    uint32_t current_key = 0x00000000;
    uint32_t* keys = calloc(13*rounds + 9, sizeof(uint32_t));
    uint32_t* dkeys = calloc(13*rounds + 9, sizeof(uint32_t));
    for (int i = 0; i < 4; ++i) {
        keys[i] = get_next_key(&current_key);
    }
    for (int i = 0; i < rounds; ++i) {
        for (int k = 0; k < 13; ++k) {
            keys[4 + i*rounds + k] = get_next_key(&current_key);
        }
    }
    for (int i = 0; i < 5; ++i) {
        keys[4 + 13*rounds + i] = get_next_key(&current_key);
    }
    gen_decrypt_keys(keys, dkeys, rounds);
    // =======================================

    input_transformation(word, keys);
    for (unsigned r = 0; r < rounds; ++r) {
        iterate(word, r + 1, keys);
    }
    output_transformation(word, keys, rounds);

    input_transformation(word, dkeys);
    for (unsigned r = 0; r < rounds; ++r) {
        iterate(word, r + 1, dkeys);
    }
    output_transformation(word, dkeys, rounds);

    printf("%s\n", (char*) word);

    free(word);

    return 0;
}