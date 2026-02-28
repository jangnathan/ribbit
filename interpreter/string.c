#include "string.h"
#include <stdlib.h>
#include <string.h>

void strings_init(strings_t *strings) {
	strings->len = 0;
	strings->size = 4;
	strings->array = malloc(sizeof(string_t) * strings->size);
}

uint16_t new_string(strings_t *strings) {
	if (strings->len >= strings->size) {
		strings->size *= 2;
		string_t *temp_ptr = realloc(strings->array, sizeof(string_t) * strings->size);
		strings->array = temp_ptr;
	}

	string_t *str = &strings->array[strings->len];
	strings->len++;

	str->len = 0;
	str->size = 8;
	str->array = malloc(sizeof(char) * str->size);
	return strings->len - 1;
}

void add_char2string_w_id(string_t *str, char ch) {
	str->array[str->len] = ch;
	str->len++;
	if (str->len >= str->size) {
		str->size *= 2;
		str->array = realloc(str->array, sizeof(char) * str->size);
	}
}

void add_strings_w_id(strings_t *str, uint32_t target_id, uint32_t model_id) {
	string_t *target = &str->array[target_id];
	string_t model = str->array[model_id];

	if (model.len + target->len > model.size) {
		if (model.size >= target->size) {
			target->size = model.size * 2;
		} else {
			target->size *= 2;
		}
		target->array = realloc(target->array, sizeof(char) * target->size);
	}

	uint16_t offset = target->len;
	for (uint16_t i = 0; i < model.len; i++) {
		target->array[i + offset] = model.array[i];
	}
	target->len = model.len + target->len;
}

void copy_string_w_id(strings_t *str, uint32_t target_id, uint32_t model_id) {
	string_t *target = &str->array[target_id];
	string_t model = str->array[model_id];

	if (model.size > target->size) {
		target->size = model.size;
		target->array = realloc(target->array, sizeof(char) * target->size);
	}

	for (int i = 0; i < model.len; i++) {
		target->array[i] = model.array[i];
	}
	target->len = model.len;
}
