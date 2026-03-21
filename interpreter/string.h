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
uint16_t new_string(strings_t *strings);

void add_char2string(string_t *str, char ch);
void add_strings_w_id(strings_t *str, uint32_t target_id, uint32_t model_id);
void copy_string_w_id(strings_t *str, uint32_t target_id, uint32_t model_id);
