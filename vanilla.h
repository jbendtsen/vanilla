#pragma once

typedef struct {
	uint32_t key;
	uint32_t value;
} KeyValue32;

typedef struct {
	KeyValue32 *pairs;
	int sizeLog2;
	int capacity;
	int occupied;
} HashTable32;
