#ifndef VESTIGE_BITSET_H
#define VESTIGE_BITSET_H

#include <stdint.h>
#include <stddef.h>

#define VESTIGE_BITSET_TYPE uint64_t
#define VESTIGE_BITSET_SIZE (8 * sizeof(VESTIGE_BITSET_TYPE))

typedef struct Bitset {
    size_t count;
    VESTIGE_BITSET_TYPE* bits;
} Bitset;

Bitset create_bitset(size_t bit_count);

void bitset_set_bit(Bitset* bitset, size_t bit, uint8_t value);
void bitset_flip_bit(Bitset* bitset, size_t bit);
uint8_t bitset_is_bit_set(Bitset* bitset, size_t bit);

size_t bitset_first_set_bit(Bitset* bitset);
size_t bitset_last_set_bit(Bitset* bitset);

#endif

