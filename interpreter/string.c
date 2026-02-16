#include "string.h"
#include <stdlib.h>

void strings_init(strings_t *strings) {
	strings->len = 0;
	strings->size = 4;
	strings->array = malloc(sizeof(string_t) * strings->size);
}

string_t *new_string(strings_t *strings) {
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
	return str;
}
void add2string(string_t *str, char ch) {
	str->array[str->len] = ch;
	str->len++;
	if (str->len >= str->size) {
		str->size *= 2;
		str->array = realloc(str->array, sizeof(char) * str->size);
	}
}
