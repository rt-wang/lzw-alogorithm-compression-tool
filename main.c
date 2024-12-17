#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h> // Include for basename
#include <stddef.h>
#include <unistd.h>

#include "encode.h"
#include "decode.h"



int maxBits = 12;
int p = 0;

#include <stdint.h>
#include <stdlib.h>

int* read_bits(char* input, int input_size, int *size) {
    if (input == NULL) {
        return NULL; // Invalid input or parameters
    }
    int temp_size = input_size*3;
    int* result = malloc(sizeof(int) * temp_size);
    if (!result) return NULL;

    uint32_t buffer = 0;
    int bit_count = 0;
    int input_pos = 1;
    int i = 0;
    int boundary = 9;

    while (input_pos < input_size) {
        while (bit_count < boundary) {
            //fprintf(stderr, "input_pos: %d\n", input[input_pos]);
            uint8_t byte = (unsigned char)input[input_pos++];
            buffer = (buffer << 8) | byte;
            bit_count += 8;
        }

        if (i >= temp_size){
            temp_size *= 2;
            int* temp = realloc(result, temp_size * sizeof(int));
            if (!temp) return NULL;
            result = temp;
        }
        
        result[i] = (buffer >> (bit_count - boundary)) & ((1 << boundary) - 1);
        if (result[i] == 256){
            boundary++;
            bit_count = boundary;
        }
        //fprintf(stderr, "input: %d, input_pos: %d, result: %d, boundary: %d\n", (unsigned char)input[input_pos-1], input_pos, result[i], boundary);
        //fprintf(stderr, "result: %d\n", result[i]);
        bit_count -= boundary;
        buffer &= ((1 << bit_count) - 1);
        i++;
    }
    *size = i;
    int* temp = realloc(result,(i+1) * sizeof(int));
    if (!temp) return NULL;
    result = temp;
    return result;
}


int main(int argc, char *argv[])
{
    static char bin[64], bout[64];
    setvbuf(stdin, bin, _IOFBF, 64);
    setvbuf(stdout, bout, _IOFBF, 64);
    int INITIAL_SIZE = 100000;

    char *exec_name = basename(argv[0]);


    if (strcmp(exec_name, "encode") == 0) {
        int opt;

        char *input = malloc(sizeof(char) * INITIAL_SIZE);
        if (input == NULL) {
            fprintf(stderr, "Failed to allocate memory\n");
            exit(1);
        }
        size_t capacity = INITIAL_SIZE;
        size_t pos = 0;
        int c;

        while ((opt = getopt(argc, argv, "m:p")) != -1) {
            switch (opt) {
                case 'm':
                    maxBits = atoi(optarg);
                    break;
                case 'p':
                    p = 1;
                    break;
                default:
                    fprintf(stderr, "Usage: %s [-m number] [-p]\n", argv[0]);
                    exit(1);
            }
        }

        //printf("p: %d, m: %d\n", pruning, maxBits);
        
        if (maxBits < 8 || maxBits > 12){
            maxBits = 12;
        }

        while ((c = getchar()) != EOF) {
            if (pos >= capacity) {
                capacity *= 2;
                char *temp = realloc(input, capacity);
                if (temp == NULL) {
                    fprintf(stderr, "Failed to reallocate memory\n");
                    free(input);
                    exit(1);
                }
                input = temp;
            }
            input[pos++] = (unsigned char)c;
        }
        
        input[pos] = '\0';
        printf("%d\n", maxBits);
        printf("%d\n", p);
        encode(p, maxBits, input, pos);
        free(input);
    } else if (strcmp(exec_name, "decode") == 0) {
        if (argc > 1) {
            fprintf(stderr, "decode: invalid option '%s'\n", argv[1]);
            exit(1);
        }
        
        char *input = malloc(sizeof(char) * INITIAL_SIZE);
        if (input == NULL) {
            fprintf(stderr, "Failed to allocate memory\n");
            exit(1);
        }
        size_t capacity = INITIAL_SIZE;
        size_t pos = 0;
        int boundary;
        int num;
        int count = 0;

        if (scanf("%d", &maxBits) != 1) {
            fprintf(stderr, "Failed to read maxBits\n");
        }
        if (scanf("%d", &p) != 1) {
            fprintf(stderr, "Failed to read maxBits\n");
        }

        fprintf(stderr, "maxBits: %d\n", maxBits);
        fprintf(stderr, "p: %d\n", p);

        int c;
        while ((c = getchar()) != EOF) {
            if (pos >= capacity) {
                capacity *= 2;
                char *temp = realloc(input, capacity);
                if (temp == NULL) {
                    fprintf(stderr, "Failed to reallocate memory\n");
                    free(input);
                    exit(1);
                }
                input = temp;
            }
            input[pos++] = (char)c;
        }

        int* unpacked = read_bits(input, pos, &count);
        if (unpacked == NULL) {
            fprintf(stderr, "Failed to read bits\n");
            free(input);
            exit(1);
        }
        unpacked[count] = '\0';

        fprintf(stderr, "size of unpacked: %d\n", count);


        decode(unpacked, count, maxBits, p);
        free(unpacked);
        free(input);
    } else {
        fprintf(stderr, "Usage: %s [-m MAXBITS] [-p] < input > output\n", argv[0]);
        fprintf(stderr, "       %s < input > output\n", argv[0]);
        exit(1);
    }
    exit(0);
}