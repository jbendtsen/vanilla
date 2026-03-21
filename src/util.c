#include "vanilla.h"

#include <stdlib.h>
#include <string.h>

// based on murmur3_32_finalize
uint32_t hash_32_32(uint32_t h) {
    h ^= h;
    h *= 0x85ebca6bU;
    h ^= h >> 13;
    h *= 0xc2b2ae35U;
    h ^= h >> 16;
    return h;
}

KeyValue32 *HashTable32_locate(HashTable32 *tbl, uint32_t key) {
    uint32_t hash = hash_32_32(key);
    int shift = tbl->sizeLog2;
    int idx = (int)(hash & ((1U << shift) - 1));

    for (int i = 0; i < (1 << shift); i++) {
        KeyValue32 *pair = &tbl->pairs[(idx + i) & ((1 << shift) - 1)];
        if (pair->key == key || pair->key == 0) {
            return pair;
        }
    }

    return NULL;
}

void HashTable32_rebalance(HashTable32 *tbl, KeyValue32 *pair) {
    if (pair) {
        if (pair < tbl->pairs || pair >= &tbl->pairs[1 << tbl->sizeLog2])
            return;
        tbl->occupied++;
    }

    if (tbl->pairs && tbl->occupied * 4 <= (1 << tbl->sizeLog2) * 3)
        return;

    int newLog2 = tbl->sizeLog2 + 1;
    int newCap = tbl->capacity;
    if (newCap < 16)
        newCap = 16;
    while (newCap < (1 << newLog2))
        newCap = ((newCap * 17) / 10) + 1;

    int oldSize = 1 << tbl->sizeLog2;
    tbl->sizeLog2 = newLog2;
    tbl->capacity = newCap;

    if (newCap > tbl->capacity) {
        KeyValue32 *oldPairs = tbl->pairs;
        tbl->pairs = calloc(newCap, sizeof(KeyValue32));
        if (oldPairs) {
            for (int i = 0; i < oldSize; i++) {
                if (oldPairs[i].key != 0) {
                    KeyValue32 *pair = HashTable32_locate(tbl, oldPairs[i].key);
                    if (pair) {
                        pair->key = oldPairs[i].key;
                        pair->value = oldPairs[i].value;
                    }
                }
            }
            free(oldPairs);
        }
    }
}

uint32_t HashTable32_put(HashTable32 *tbl, uint32_t key, uint32_t value) {
    KeyValue32 *pair = HashTable32_locate(tbl, key);
    if (!pair) {
        HashTable32_rebalance(tbl, NULL);
        pair = HashTable32_locate(tbl, key);
        if (!pair)
            return 0;
    }

    uint32_t oldValue = 0;
    if (pair->key == key)
        oldValue = pair->value;

    pair->key = key;
    pair->value = value;

    HashTable32_rebalance(tbl, pair);
}

int HashTable32_get(HashTable32 *tbl, uint32_t key, uint32_t *outValue) {
    KeyValue32 *pair = HashTable32_locate(tbl, key);
    if (pair && pair->key == key) {
        *outValue = pair->value;
        return 1;
    }
    return 0;
}

#define IMPL_VECTOR(pref, type) \
void pref ## Vector_resize(pref ## Vector *vec, int newSize) { \
    if (newSize < vec->capacity) { \
        vec->size = newSize; \
        return; \
    } \
    int newCap = vec->capacity; \
    if (newCap < 16) \
        newCap = 16; \
    while (newCap < newSize) \
        newCap = ((newCap * 17) / 10) + 1; \
    if (newCap > vec->capacity) { \
        type *newData = (type*)malloc(newCap * sizeof(type)); \
        if (vec->data != NULL) { \
            memcpy(newData, vec->data, vec->size * sizeof(type)); \
            free(vec->data); \
        } \
        vec->data = newData; \
        vec->capacity = newCap; \
    } \
    vec->size = newSize; \
} \
void pref ## Vector_add(pref ## Vector *vec, type elem) { \
    int pos = vec->size; \
    ByteVector_resize(vec, pos + 1); \
    vec->data[pos] = elem; \
} \
void pref ## Vector_extend(pref ## Vector *vec, type *data, int sz) { \
    if (sz > 0) { \
        int pos = vec->size; \
        ByteVector_resize(vec, pos + sz); \
        memcpy(&vec[pos], data, sz * sizeof(type)); \
    } \
}

IMPL_VECTOR(Byte, uint8_t)
