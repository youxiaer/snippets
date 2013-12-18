#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitset.h"

void _bitset_resize(s_bitset *bitset, size_t len) {
    size_t empty_next_size, cur_next_size; 
    S_BITSET_NEXT_POW_2(empty_next_size,len);
    if (!bitset->bits_num) {
        bitset->slots = malloc(S_BITSET_BYTE_LENGTH(empty_next_size));
    } else {
        S_BITSET_NEXT_POW_2(cur_next_size,bitset->bits_num);
        if (empty_next_size > cur_next_size) {
            bitset->slots = realloc(bitset->slots, S_BITSET_BYTE_LENGTH(empty_next_size));
        }
    }
    if (!bitset->slots) {
        S_BITSET_FATAL("out of memory");
    }
    bitset->bits_num = len;
}
s_bitset *s_bitset_create() {
    s_bitset *bitset = malloc(sizeof(s_bitset));
    if (!bitset) {
        S_BITSET_FATAL("out of memory");
    }
    bitset->slots = malloc(sizeof(s_bitset_slot));
    if (!bitset->slots) {
        S_BITSET_FATAL("out of memory");
    }
    bitset->bits_num = S_BITSET_DEFAULT_BITS;
    bitset->bits_seted = 0;
    return bitset;
}

void s_bitset_free(s_bitset *bitset) {
    if (bitset->slots) {
        free(bitset->slots);
    }
    free(bitset);
}

size_t s_bitset_length(const s_bitset *bitset) {
    return S_BITSET_BYTE_LENGTH(bitset->bits_num - 1);
}

 s_bitset *s_bitset_copy(const s_bitset *bitset) {
    s_bitset *copy = malloc(sizeof(s_bitset));
    if (!copy) {
        S_BITSET_FATAL("out of memory");
    }
    if (bitset->slots) {
        size_t length = s_bitset_length(bitset);
        copy->slots = malloc(length);
        if (!copy->slots) {
            S_BITSET_FATAL("out of memory");
        }
        memcpy(copy->slots, bitset->slots, length);
        copy->bits_num = bitset->bits_num;
        copy->bits_seted = bitset->bits_seted;
    }
    return copy;
 }

 bool s_bitset_any(const s_bitset *bitset) {
    return bitset->bits_seted ? true : false;
 }

 size_t s_bitset_count(const s_bitset *bitset) {
    return bitset->bits_seted;
 }

void s_bitset_flip_pos(s_bitset *bitset, size_t pos) {
    if (pos > (bitset->bits_num - 1)) {
        S_BITSET_FATAL("pos is greater than the size of bitset");
    }
    bool is_set = s_bitset_test(bitset, pos);
    bitset->slots[pos >> 5] ^= (1 << (pos & S_BITSET_SLOT_MASK));
    if (is_set) {
        --bitset->bits_seted;
    } else {
        ++bitset->bits_seted;
    }
}

void s_bitset_flip(s_bitset *bitset) {
    int slot = S_BITSET_SLOT_NUM(bitset->bits_num);
    int i;
    for (i = 0; i < slot; i++) {
        bitset->slots[i] ^= S_BITSET_ALL_TRUE_MASK;
    }
    int ext = bitset->bits_num & S_BITSET_SLOT_MASK;
    if (ext) {
        bitset->slots[slot - 1] &=  (S_BITSET_ALL_TRUE_MASK >> (S_BITSET_SLOT_LENGTH - ext));
    }
    bitset->bits_seted = bitset->bits_num - bitset->bits_seted;
}

bool s_bitset_none(const s_bitset *bitset) {
    return bitset->bits_seted ? false : true;
}

void s_bitset_reset_pos(s_bitset *bitset, size_t pos) {
    if (pos < (bitset->bits_num)) {
        bool is_set = s_bitset_test(bitset, pos);
        if (is_set) {
            bitset->slots[pos >> 5] &= ((S_BITSET_ALL_TRUE_MASK << (pos & S_BITSET_SLOT_MASK)) -1);
            --bitset->bits_seted;
        }
    }
}

void s_bitset_reset(s_bitset *bitset) {
    int slot = S_BITSET_SLOT_NUM(bitset->bits_num);
    int i;
    for (i = 0; i < slot; i++) {
        bitset->slots[i] &= S_BITSET_ALL_FALSE_MASK;
    }
    bitset->bits_seted = 0;
}

void s_bitset_set_pos(s_bitset *bitset, size_t pos) {
    if (pos > (bitset->bits_num - 1)) {
        _bitset_resize(bitset, pos);
    }
    bitset->slots[pos >> 5] |= (1 << (pos & S_BITSET_SLOT_MASK));
    ++bitset->bits_seted;
}

void s_bitset_set(s_bitset *bitset) {
    int slot = S_BITSET_SLOT_NUM(bitset->bits_num);
    int i;
    for (i = 0; i < slot; i++) {
        bitset->slots[i] |= S_BITSET_ALL_TRUE_MASK;
    }
    int ext = bitset->bits_num & S_BITSET_SLOT_MASK;
    if (ext) {
        bitset->slots[slot - 1] &=  (S_BITSET_ALL_TRUE_MASK >> (S_BITSET_SLOT_LENGTH - ext));
    }
    bitset->bits_seted = bitset->bits_num;
}

bool s_bitset_test(const s_bitset *bitset, size_t pos) {
    if (pos >= (bitset->bits_num)) {
        return false;
    }
    size_t is_set = bitset->slots[pos >> 5] & (1 << (pos & S_BITSET_SLOT_MASK));
    return is_set ? true : false;
}

char *s_bitset_to_string(const s_bitset *bitset) {
    if (bitset->bits_num == 0) {
        return NULL;
    } else {
        char *rs = (char *)malloc(bitset->bits_num);
        int i = 0;
        for (i = 0; i < bitset->bits_num; i++) {
            if (s_bitset_test(bitset, i)) {
                rs[i] = '1';
            } else {
                rs[i] = '0';
            }
        }
        return rs;
    }
}

size_t s_bitset_to_ulong(const s_bitset *bitset) {
    int slot = S_BITSET_SLOT_NUM(bitset->bits_num);
    int i;
    for (i = slot - 1; i > 0; --i) {
        if (bitset->slots[i]) {
            S_BITSET_FATAL("overflow");
        }
    }
    return bitset->slots[0];
}

