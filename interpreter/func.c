#include "func.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void func_add_param(func_t *func, char *name) {
	strcpy(func->params[func->n_param].name, name);
	func->n_param++;
}

func_t *new_func(funcs_t *funcs, char *name) {
	if (funcs->len > funcs->size) {
		funcs->size *= 2;
		funcs->array = realloc(funcs->array, sizeof(func_t) * funcs->size);
	}

	func_t *func = &funcs->array[funcs->len];
	funcs->len++;

	strcpy(func->name, name);
	func->n_param = 0;
	return func;
}

void funcs_init(funcs_t *funcs) {
	funcs->len = 0;
	funcs->size = 4;
	funcs->array = malloc(sizeof(func_t) * funcs->size);

	func_t *print = new_func(funcs, "print");
	func_add_param(print, "content");
}

func_t *get_func(funcs_t *funcs, char *name) {
	uint8_t found = 0;
	uint16_t i = 0;
	while (!found && i < funcs->len) {
		if (strcmp(funcs->array[i].name, name) == 0) {
			found = 1;
		}
		i++;
	}
	if (found) {
		return &funcs->array[i - 1];
	} else {
		return 0;
	}
}
