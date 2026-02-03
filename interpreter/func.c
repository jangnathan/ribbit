#include "func.h"

void add_func(func_t *funcs, char *name, node_t *ptr) {
	funcs->array[funcs->len] = print;
	funcs->len++;
}

void funcs_init(func_t *funcs) {
	funcs->len = 0;
	funcs->size = 4;
	funcs->array = malloc(sizeof(func_t) * funcs->size);

	add_func(funcs, "print", 0);
}

func_t *get_func(func_t *funcs, char *name) {
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
