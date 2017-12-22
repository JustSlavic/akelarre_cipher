#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "akelarre.h"
#include "keygen.h"

#define input_file "message.txt"
#define output_file "cipher"
#define word_size 16
#define rounds 1

// usage: akelarre {encrypt|decrypt} input_file key_file output_file
int main(int argc, char** argv) {

    if (argc < 3) {
        printf("usage: akelarre {encrypt|decrypt} input_file key_file output_file\n");
        return 1;
    }

    if (strcmp(argv[1],"encrypt") == 0) {
        // ======== generate key schedule ========
        srand(time(NULL));
        uint32_t current_key = rand() % INT32_MAX; // gen random key
        uint32_t n_keys = 13*rounds + 9;
        uint32_t* keys = calloc(n_keys, sizeof(uint32_t));

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

        FILE* kFile = fopen(argv[3], "wb");
        if (!kFile) {
            printf("bad key file\n");
            return 1;
        }

        fwrite(&n_keys, sizeof(uint32_t), 1, kFile);
        fwrite(keys, sizeof(uint32_t), n_keys, kFile);

        fclose(kFile);
        // =======================================

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

        char* buffer = calloc(word_size, sizeof(char));
        PWord word = (PWord) buffer;

        while (!feof(textFile)) {
            memset(buffer, 0, word_size*sizeof(char));
            fread(buffer, sizeof(char), word_size, textFile);

            input_transformation(word, keys);
            for (unsigned r = 0; r < rounds; ++r) {
                iterate(word, r + 1, keys);
            }
            output_transformation(word, keys, rounds);

            fwrite(buffer, sizeof(char), word_size, cipherFile);
        }

        fclose(textFile);
        fclose(cipherFile);
        free(buffer);
        free(keys);

    } else if (strcmp(argv[1],"decrypt") == 0) {
        // =========== read keys and make decrypt keys =========
        FILE* kFile = fopen(argv[3], "rb");

        uint32_t n_keys = 0;
        fread(&n_keys, sizeof(uint32_t), 1, kFile);

        uint32_t* keys = calloc(n_keys, sizeof(uint32_t));
        uint32_t* dkeys = calloc(n_keys, sizeof(uint32_t));

        fread(keys, sizeof(uint32_t), n_keys, kFile);
        gen_decrypt_keys(keys, dkeys, rounds);

        fclose(kFile);
        // =====================================================

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

        char* buffer = calloc(word_size, sizeof(char));
        PWord word = (PWord) buffer;

        while (!feof(cipherFile)) {
            memset(buffer, 0, word_size*sizeof(char));
            fread(buffer, sizeof(char), word_size, cipherFile);

            input_transformation(word, dkeys);
            for (unsigned r = 0; r < rounds; ++r) {
                iterate(word, r + 1, dkeys);
            }
            output_transformation(word, dkeys, rounds);

            printf("%s", buffer);

            fwrite(buffer, sizeof(char), word_size, textFile);
        }

        fclose(textFile);
        fclose(cipherFile);
        free(buffer);
        free(keys);
        free(dkeys);

    } else {
        printf("usage: akelarre {encrypt|decrypt} input_file key_file output_file\n");
        return 1;
    }

    return 0;
}