#pragma once
#include "constants.h"
#include "value.h"

#include <stdint.h>

typedef struct {
	char name[MAX_LEX_LEN];
	uint16_t ptr;
} var_t;

typedef struct {
	var_t *array;
	uint16_t len;
	uint16_t size;
} vars_t;

uint16_t new_var(vars_t *vars, char *name);
void vars_init(vars_t *vars);
int32_t get_var(vars_t *vars, char *name);
