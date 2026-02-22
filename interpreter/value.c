#include "value.h"

#include <stdlib.h>

void values_init(values_t *values) {
	values->len = 0;
	values->size = 4;
	values->array = malloc(sizeof(value_t) * values->size);
}
uint16_t new_value(values_t *values) {
	if (values->len >= values->size) {
		values->size *= 2;
		values->array = realloc(values->array, sizeof(value_t) * values->size);
	}

	values->array[values->len].type = UNDEFINED;
	values->len++;

	return values->len - 1;
}
