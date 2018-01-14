#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "akelarre.h"
#include "keygen.h"

#define WORD_SIZE 16

// usage: akelarre {encrypt|decrypt} input_file key_file output_file [r]
int main(int argc, char** argv) {

    if (argc < 5) {
        printf("usage: akelarre {encrypt|decrypt} input_file key_file output_file [r]\n");
        return 1;
    }

    uint32_t rounds = 1;
    if (argc == 6) {
        rounds = (uint32_t) strtol(argv[5], (argv+6), 10);
    }

    // =========== read master key =========
    uint32_t n_keys = 13*rounds + 9;
    uint16_t* master_key = calloc(8, sizeof(uint16_t)); // 128 bit of master key

    FILE* kFile = fopen(argv[3], "rb");
    if (!kFile) {
        printf("bad key file\n");
        return 1;
    }

    fread(master_key, sizeof(uint16_t), 8, kFile);

    fclose(kFile);
    //======================================


    if (strcmp(argv[1],"encrypt") == 0) {

        uint32_t* keys = calloc(n_keys, sizeof(uint32_t));
        gen_encrypt_keys(master_key, keys, rounds);

        FILE* textFile = fopen(argv[2], "rb"); //open input_file
        if (!textFile) {
            printf("bad text file\n");
            return 1;
        }
        FILE* cipherFile = fopen(argv[4], "wb"); // open output_file
        if (!cipherFile) {
            printf("bad cipher file\n");
            return 1;
        }

        char* buffer = calloc(WORD_SIZE, sizeof(char));
        PWord word = (PWord) buffer;

        while (!feof(textFile)) {
            memset(buffer, 0, WORD_SIZE*sizeof(char));
            fread(buffer, sizeof(char), WORD_SIZE, textFile);

            input_transformation(word, keys);
            for (unsigned r = 0; r < rounds; ++r) {
                iterate(word, r + 1, keys);
            }
            output_transformation(word, keys, rounds);

            fwrite(buffer, sizeof(char), WORD_SIZE, cipherFile);
        }

        for (int i = 0; i < n_keys; ++i) {
            printf("%X\n", keys[i]);
        }

        fclose(textFile);
        fclose(cipherFile);
        free(buffer);
        free(keys);

    } else if (strcmp(argv[1],"decrypt") == 0) {
        uint32_t* dkeys = calloc(n_keys, sizeof(uint32_t));
        gen_decrypt_keys(master_key, dkeys, rounds);

        FILE* cipherFile = fopen(argv[2], "rb"); //open input_file
        if (!cipherFile) {
            printf("bad cipher file\n");
            return 1;
        }

        FILE* textFile = fopen(argv[4], "wb"); //open output_file
        if (!textFile) {
            printf("bad text file\n");
            return 1;
        }

        char* buffer = calloc(WORD_SIZE, sizeof(char));
        PWord word = (PWord) buffer;

        while (!feof(cipherFile)) {
            memset(buffer, 0, WORD_SIZE*sizeof(char));
            fread(buffer, sizeof(char), WORD_SIZE, cipherFile);

            input_transformation(word, dkeys);
            for (unsigned r = 0; r < rounds; ++r) {
                iterate(word, r + 1, dkeys);
            }
            output_transformation(word, dkeys, rounds);

            fwrite(buffer, sizeof(char), WORD_SIZE, textFile);
        }

        fclose(textFile);
        fclose(cipherFile);
        free(buffer);
        free(dkeys);

    } else {
        printf("usage: akelarre {encrypt|decrypt} input_file key_file output_file [r]\n");
        return 1;
    }

    return 0;
}