#pragma once
#include <stdint.h>

typedef struct {
	char *array;
	uint16_t len;
	uint16_t size;
} string_t;

typedef struct {
	string_t *array;
	uint32_t q[32];
	uint32_t len;
	uint32_t size;
	uint8_t q_len;
} strings_t;

void strings_init(strings_t *strings);
uint16_t new_string(strings_t *strings);

void delete_string(strings_t *strings, uint32_t id);
void add_char2string(string_t *str, char ch);
void add_strings_w_id(strings_t *str, uint32_t target_id, uint32_t model_id);
void copy_string_w_id(strings_t *str, uint32_t target_id, uint32_t model_id);
uint8_t equal_string_w_id(strings_t *str, uint32_t left_id, uint32_t right_id);
