#include <stdlib.h>

#include "lib/bitset.h"

Bitset create_bitset(size_t bit_count) {
    Bitset bits;

    bits.count = bit_count;
    bits.bits = malloc(bit_count * VESTIGE_BITSET_SIZE);

    return bits;
}

void bitset_set_bit(Bitset* bitset, size_t bit, uint8_t value) {
    size_t subbitset = bit / VESTIGE_BITSET_SIZE;
    size_t index = bit % VESTIGE_BITSET_SIZE;

    // Desired:
    // if bitset[bit] == 0 and value == 0 then bitset[bit] == 0
    // if bitset[bit] == 1 and value == 0 then bitset[bit] == 0
    // if bitset[bit] == 0 and value == 1 then bitset[bit] == 1
    // if bitset[bit] == 1 and value == 1 then bitset[bit] == 1

    value = value != 0;
    uint8_t current = bitset->bits[subbitset] & ((VESTIGE_BITSET_TYPE)1 << index);

    // if bitset[bit] == 0 and value == 0 then value = 0
    // if bitset[bit] == 1 and value == 0 then value = 1
    // if bitset[bit] == 0 and value == 1 then value = 1
    // if bitset[bit] == 1 and value == 1 then value = 0
    value = value != current;

    // if bitset[bit] == 0 and value == 0 then value = 0
    // if bitset[bit] == 1 and value == 0 then value = 0
    // if bitset[bit] == 0 and value == 1 then value = 1
    // if bitset[bit] == 1 and value == 1 then value = 1
    bitset->bits[subbitset] ^= ((VESTIGE_BITSET_TYPE)value << index);
}

void bitset_flip_bit(Bitset* bitset, size_t bit) {
    size_t subbitset = bit / VESTIGE_BITSET_SIZE;
    size_t index = bit % VESTIGE_BITSET_SIZE;

    bitset->bits[subbitset] ^= ((VESTIGE_BITSET_TYPE)1 << index);
}

uint8_t bitset_is_bit_set(Bitset* bitset, size_t bit) {
    size_t subbitset = bit / VESTIGE_BITSET_SIZE;
    size_t index = bit % VESTIGE_BITSET_SIZE;

    return 0 != (bitset->bits[subbitset] & ((VESTIGE_BITSET_TYPE)1 << index));
}

size_t bitset_first_set_bit(Bitset* bitset) {
    for (size_t i = 0; i < bitset->count; i++) {
        VESTIGE_BITSET_TYPE bits = bitset->bits[i];
        if (bits != ~((VESTIGE_BITSET_TYPE)0)) {
            return __builtin_ffsll(~bits) - 1;
        }
    }
    return bitset->count;
}

size_t bitset_last_set_bit(Bitset* bitset) {
    for (size_t i = 0; i < bitset->count; i++) {
        VESTIGE_BITSET_TYPE bits = bitset->bits[bitset->count - i - 1];
        if (bits != ~((VESTIGE_BITSET_TYPE)0)) {
            return VESTIGE_BITSET_SIZE - __builtin_clzll(~bits) - 1;
        }
    }
    return 0;
}
