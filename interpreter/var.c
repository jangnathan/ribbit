#include "var.h"

#include <stdlib.h>
#include <string.h>

var_t *new_var(vars_t *vars, char *name) {
	var_t var;
	strcpy(var.name, name);
	vars->array[vars->len] = var;

	vars->len++;
	if (vars->len > vars->size) {
		vars->size *= 2;
		vars->array = realloc(vars->array, sizeof(var_t) * vars->size);
	}
	return &vars->array[vars->len - 1];
}
void vars_init(vars_t *vars) {
	vars->len = 0;
	vars->size = 8;
	vars->array = malloc(sizeof(var_t) * vars->size);
}
var_t *get_var(vars_t *vars, char *name) {
	uint8_t found = 0;
	uint8_t i = 0;
	while (!found && i < vars->len) {
		if (strcmp(vars->array[i].name, name) == 0) {
			found = 0;
		}
		i++;
	}
	if (found) {
		return &vars->array[i];
	} else {
		return 0;
	}
}
