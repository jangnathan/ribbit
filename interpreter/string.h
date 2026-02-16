#pragma once
#include <stdint.h>

typedef struct {
	char *array;
	uint16_t len;
	uint16_t size;
} string_t;

typedef struct {
	string_t *array;
	uint16_t len;
	uint16_t size;
} strings_t;

void strings_init(strings_t *strings);
string_t *new_string(strings_t *strings);
void add2string(string_t *str, char ch);
