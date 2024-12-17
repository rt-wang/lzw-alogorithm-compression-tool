#include "encode.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <stddef.h>
#include <stdint.h>

void put_map(map* m, int prefix, int curr){
    node* n = (node*)calloc(1, sizeof(node));
    if (n == NULL) return;
    
    n->prefix = prefix;
    n->curr = curr;
    m->array[m->size] = n;
    m->size += 1;
}

int get_map(map* m, int prefix){
    for (int i = 0; i < m->size; i++){
        node* n = m->array[i];
        if (n->prefix == prefix){
            return n->curr;
        }
    }
    return 0;
}

void show_map(map* m){
    for (int i = 0; i < m->size; i++){
        fprintf(stderr,"node: %d, prefix: %d, current_char:%d\n", i, m->array[i]->prefix, m->array[i]->curr);
    }
}

void free_map(map* m){
    for (int i = 0; i < m->size; i++){
        free(m->array[i]);
    }
    free(m->array);
    free(m);
}

int search_map(int prefix, int curr, map* m) {
    if (m == NULL) return -1;
    
    int size = m->size;
    for (int i = 0; i < size; i++) {
        if (m->array[i] == NULL) continue;
        if (m->array[i]->prefix == prefix && m->array[i]->curr == curr) {
            return i;
        }
    }
    return -1;
}

map* create_map(int initial_capacity){
    map* m = (map*)malloc(sizeof(map));
    m->size = 0;
    m->capacity = initial_capacity;
    m->array = (node**)malloc(initial_capacity * sizeof(node*));
    for (int i=0; i<256; i++){
        put_map(m, -1, i);
    }
    put_map(m, 256, 256);
    return m;
}

map* pruned_map(map* m, int maxBits) {

    int count = 0;
    if (m == NULL) return NULL;
    
    int capacity = 1 << maxBits;
    map* temp = (map*)calloc(1, sizeof(map));
    if (temp == NULL) return NULL;
    
    temp->capacity = capacity;
    temp->array = (node**)calloc(capacity, sizeof(node*));
    if (temp->array == NULL) {
        free(temp);
        return NULL;
    }

    int* mapping = calloc(capacity,sizeof(int));
    
    int old_size = capacity;
    for (int i = 256; i < old_size; i++) {
        if (m->array[i] == NULL) continue;
        for (int j = i; j < old_size; j++) {
            if (m->array[j] == NULL) continue;
            if (m->array[j]->prefix == i) {
                mapping[i] = count;
                put_map(temp, m->array[i]->prefix, m->array[i]->curr);
                count++;
                break;
            }
        }
    }
    
    map *new_m = put_pruned_map(temp, maxBits, mapping);
    free_map(temp);
    free(mapping);
    return new_m;
}

map* put_pruned_map(map* temp, int maxBits, int* mapping) {
    if (temp == NULL) return NULL;
    
    int capacity = 1 << maxBits;
    map *new_m = create_map(capacity);
    if (new_m == NULL) return NULL;
    
    int process_size = temp->size;
    for (int i = 0; i < process_size; i++) {
        if (temp->array[i] == NULL) continue;
        
        node* temp_node = temp->array[i];
        int new_prefix;
        
        if (temp_node->prefix < 256) {
            new_prefix = temp_node->prefix;
        } else {
            new_prefix = mapping[temp_node->prefix];
        }
        
        if (new_m->size >= capacity) {
            free_map(new_m);
            return NULL;
        }
        
        put_map(new_m, new_prefix, temp_node->curr);
    }
    
    return new_m;
}

int find_code_in_new_map(map* old_map, map* new_map, int old_code) {
    int prefix = old_map->array[old_code]->prefix;
    int curr = old_map->array[old_code]->curr;
    return search_map(prefix, curr, new_map);
}

void write_code(int* result, int size, int boundary) {
    uint32_t buffer = 0;
    int bit_count = 0;

    for (int i = 0; i < size; i++) {
        buffer = (buffer << boundary) | (result[i] & ((1 << boundary) - 1));
        bit_count += boundary;

        while (bit_count >= 8) {
            uint8_t byte = (buffer >> (bit_count - 8)) & 0xFF;
            //fprintf(stderr, "input: %d, result: %d, boundary: %d\n", byte, result[i], boundary);
            putchar(byte);
            bit_count -= 8;
        }
    }

    if (bit_count > 0) {
        uint8_t byte = (buffer << (8 - bit_count)) & 0xFF;
        putchar(byte);
    }
}



void encode(int p, int maxBits, char *input, size_t size) {
    //fprintf(stderr, "DEBUG: Input size: %ld\n", size);

    int* result = malloc(size * sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Failed to allocate memory for result\n");
        return;
    }
    map* m = create_map(1<<maxBits);
    if (m == NULL) {
        fprintf(stderr, "Failed to create map\n");
        return;
    }
    
    int i = 0;
    int count = 0;
    int c = -1;
    int over_capacity = 0;
    int just_pruned = 0;
    int index;
    int boundary = 9;
    int total_count = 0;
    //int k = 0;

    while (i < size) {
        int cur_char = (unsigned char)input[i];
        //if (m->size >= (m->capacity-1)||m->size == 327){
            //printf("m:size->%d\n",m->size);
            //printf("c:%d, cur_char:%c\n",c, cur_char);
        //}
        if (just_pruned == 1){
            index = -1;
            just_pruned = 0;
        } else{
            index = search_map(c, cur_char, m);
        }
        if (index != -1) {
            c = index;
        } else {
            if (c >= (1<<boundary) && boundary < maxBits){
                result[count] = 256;
                count++;
                write_code(result, count, boundary);
                boundary++;
                total_count += count;
                count = 0;
                free(result);
                result = malloc(size * sizeof(int));
            }
            //printf("%d\n",c);
            result[count] = c;
            count++;
            if (!over_capacity) {
                if (m->size < m->capacity) {
                    put_map(m, c, cur_char);
                } else {
                    if (p==1){
                        map* new_m = pruned_map(m, maxBits);
                        //c = find_code_in_new_map(m,new_m,c);
                        //put_map(new_m, c, cur_char);
                        just_pruned = 1;
                        if (getenv("DEBUG")){
                            show_map(m);
                            printf("\n");
                        }
                        free_map(m);
                        m = new_m;
                    } else {
                        over_capacity = 1;
                    }
                }
            }
            c = search_map(-1, cur_char, m);
        }
        i++;
    }

    if (c!=-1){
        result[count] = c;
        count++;
    }
    total_count += count;
    result = realloc(result, total_count * sizeof(int));
    
    write_code(result, count, boundary);
    fprintf(stderr, "size: %d\n", total_count);
    
    free_map(m);
    free(result);
}


