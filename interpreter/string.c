#include "string.h"

string_t *new_string() {
	str.len = 0;
	str.size = 8;
	string_t str = malloc(sizeof(char) * str.size);
}
void add2string(string_t *str, char ch) {
	str[str->len] = ch;
	str->len++;
	if (str->len > str->size) {
		str->size *= 2;
		str = realloc(str, sizeof(char) * str->size);
	}
}
