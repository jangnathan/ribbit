#pragma once
#include <stdint.h>

typedef struct {
	char *array;
	uint16_t len;
	uint16_t size;
} string_t;

string_t *new_string();
void add2string(string_t *str, char ch);
