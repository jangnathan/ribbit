#include "var.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint16_t new_var(vars_t *vars, char *name) {
	var_t *var = &vars->array[vars->len];
	strcpy(var->name, name);

	vars->len++;
	if (vars->len > vars->size) {
		vars->size *= 2;
		vars->array = realloc(vars->array, sizeof(var_t) * vars->size);
	}
	return vars->len - 1;
}
void vars_init(vars_t *vars) {
	vars->len = 0;
	vars->size = 8;
	vars->array = malloc(sizeof(var_t) * vars->size);
}
int32_t get_var(vars_t *vars, char *name) {
	uint8_t found = 0;
	uint8_t i = 0;
	while (!found && i < vars->len) {
		if (strcmp(vars->array[i].name, name) == 0) {
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
