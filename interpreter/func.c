#include "func.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void func_add_param(func_t *func, char *name) {
	strcpy(func->params[func->n_param].name, name);
	func->n_param++;
}

uint16_t new_func(funcs_t *funcs, char *name) {
	func_t *func = &funcs->array[funcs->len];
	funcs->len++;

	strcpy(func->name, name);
	func->n_param = 0;

	if (funcs->len >= funcs->size) {
		funcs->size *= 2;
		funcs->array = realloc(funcs->array, sizeof(func_t) * funcs->size);
	}
	return funcs->len - 1;
}

void funcs_init(funcs_t *funcs) {
	funcs->len = 0;
	funcs->size = 8;
	funcs->array = malloc(sizeof(func_t) * funcs->size);

	// prints
	func_t *print = &funcs->array[new_func(funcs, "print")];
	func_add_param(print, "content");
	// input
	func_t *ask = &funcs->array[new_func(funcs, "ask")];
	func_add_param(ask, "question");
	// turn number into string, or bool into string
	func_t *str = &funcs->array[new_func(funcs, "str")];
	func_add_param(str, "number");

	// turns str into int, naming collisions with keyword made it int_func
	func_t *int_func = &funcs->array[new_func(funcs, "int")];
	func_add_param(int_func, "str");

	// no params
	func_t *exit = &funcs->array[new_func(funcs, "exit")];
}

int32_t get_func(funcs_t *funcs, char *name) {
	uint8_t found = 0;
	uint16_t i = 0;
	while (!found && i < funcs->len) {
		if (strcmp(funcs->array[i].name, name) == 0) {
			found = 1;
		}
		i++;
	}
	if (found) {
		return i - 1;
	} else {
		return -1;
	}
}
