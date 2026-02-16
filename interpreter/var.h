#pragma once
#include "constants.h"
#include "value.h"

#include <stdint.h>

typedef struct {
	char name[MAX_LEX_LEN];
	value_t value;
} var_t;

typedef struct {
	var_t *array;
	uint32_t len;
	uint32_t size;
} vars_t;

var_t *new_var(vars_t *vars, char *name);
void vars_init(vars_t *vars);
var_t *get_var(vars_t *vars, char *name);
