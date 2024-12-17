#ifndef AB1D5576_F986_4B13_AE08_ABCF50CDF907
#define AB1D5576_F986_4B13_AE08_ABCF50CDF907
#include <stddef.h>

typedef struct node {
    int prefix;
    int curr;
} node;

typedef struct map {
    int size;
    int capacity;
    node** array;
    int* old_to_new;
} map;

void encode(int p, int max_bit_length, char *input, size_t size);
void put_map(map* m, int prefix, int curr);
int get_map(map* m, int prefix);
void free_map(map* m);
int search_map(int prefix, int curr, map* m);
void show_map(map* m);
map* pruned_map(map* m, int max_bit_length);
map* put_pruned_map(map* temp, int max_bit_length, int* mapping);
map* create_map(int initial_capacity);
int find_code_in_new_map(map* old_map, map* new_map, int code);

void write_code(int* result, int size, int boundary);

extern int maxBits;

#endif /* AB1D5576_F986_4B13_AE08_ABCF50CDF907 */