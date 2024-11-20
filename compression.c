#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


#define HASH_SIZE 65536

//Hashset supports distinct value finding in pdf file
typedef struct {
    uint32_t* values;
    bool* used;
    size_t count;
} HashSet;

void init_hashset(HashSet* set) {
    set->values = calloc(HASH_SIZE, sizeof(uint32_t));
    set->used = calloc(HASH_SIZE, sizeof(bool));
    set->count = 0;
}

void free_hashset(HashSet* set) {
    free(set->values);
    free(set->used);
}

size_t hash_function(uint32_t value) {
    return (value * 2654435761U) % HASH_SIZE;  // Knuth's multiplicative hash
}

bool insert_value(HashSet* set, uint32_t value) {
    size_t index = hash_function(value);
    
    while (set->used[index]) {
        if (set->values[index] == value) {
            return false;  // Already exists
        }
        index = (index + 1) % HASH_SIZE;  // Linear probing
    }
    
    set->values[index] = value;
    set->used[index] = true;
    set->count++;
    return true;
}


//THE ALGORITHM
typedef struct{
    size_t capacity;
    size_t* bs_size; //bitstring size
    uint16_t tail; //last index of array
    uint32_t* values; // array of values
} HashMap;

void alloc_bs_size(HashMap* map, size_t* size){
    map -> bs_size = size;
};
void alloc_capacity(HashMap* map, size_t* distinct_v){
    map -> capacity = *distinct_v;
}
void alloc_val(HashMap* map, uint32_t val){
    uint16_t last = map->tail++;
    map->values[last] = val;
}

// For the case that we can stuff the values into 1 byte
// Get nth bit from array
int gb(unsigned char* array, size_t n) {
    return (array[n/8] >> (7 - (n%8))) & 1;
}

// Set nth bit in array
void sb(unsigned char* array, size_t n, int val) {
    if (val)
        array[n/8] |= 1 << (7 - (n%8));
    else
        array[n/8] &= ~(1 << (7 - (n%8)));
}


void write_bits(unsigned char* out, size_t* bit_pos, unsigned int val, int bits) {
    size_t byte_pos = *bit_pos / 8;
    int bit_offset = *bit_pos % 8;

    val &= (1 << bits) - 1;
    out[byte_pos] |= val << bit_offset;
    if (bit_offset + bits > 8) {
        out[byte_pos + 1] |= val >> (8 - bit_offset);
    }
    *bit_pos += bits;
}

void get_distinct_values(uint32_t* pdf_data, size_t length, HashMap* map) {
    HashSet set;
    init_hashset(&set);
    
    for (size_t i = 0; i < length; i++) {
        insert_value(&set, pdf_data[i]);
    }
    
    // Allocate map with found distinct values
    alloc_capacity(map, &set.count);
    map->values = malloc(set.count * sizeof(uint32_t));
    map->tail = 0;
    
    // Copy distinct values to map
    for (size_t i = 0; i < HASH_SIZE; i++) {
        if (set.used[i]) {
            alloc_val(map, set.values[i]);
        }
    }
    
    free_hashset(&set);
}

//THE ALGORITHM END


unsigned char* file_to_bits(const char* path, size_t* bit_count) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* buffer = malloc(file_size);
    fread(buffer, 1, file_size, fp);
    fclose(fp);

    *bit_count = file_size * 8;
    return buffer;
}

int main(){
    return 0;
}
