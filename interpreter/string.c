#include "string.h"
#include <stdlib.h>

string_t *new_string() {
	string_t *str = malloc(sizeof(string_t));
	str->len = 0;
	str->size = 8;
	str->array = malloc(sizeof(char) * str->size);
	return str;
}
void add2string(string_t *str, char ch) {
	str->array[str->len] = ch;
	str->len++;
	if (str->len > str->size) {
		str->size *= 2;
		str->array = realloc(str->array, sizeof(char) * str->size);
	}
}
