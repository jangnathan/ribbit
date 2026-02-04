#include "func.h"

void add_func(funsc_t *funcs, char *name, node_t *ptr) {
	func_t func;
	func->name = name;
	func->ptr = ptr;
	funcs->array[funcs->len] = func;

	funcs->len++;
	if (funcs->len > funcs->size) {
		funcs->size *= 2;
		funcs->array = realloc(funcs->array, sizeof(func_t) * funcs->size);
	}
}

void funcs_init(funcs_t *funcs) {
	funcs->len = 0;
	funcs->size = 4;
	funcs->array = malloc(sizeof(func_t) * funcs->size);

	add_func(funcs, "print", 0);
}

func_t *get_func(funcs_t *funcs, char *name) {
	uint8_t found = 0;
	uint8_t i = 0;
	while (!found && i < funcs->len) {
		if (strcmp(funcs->array[i].name, name) == 0) {
			found = 0;
		}
		i++;
	}
	if (found) {
		return funcs->array[i];
	} else {
		return 0;
	}
}
