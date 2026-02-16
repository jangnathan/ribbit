#pragma once
#include "ast.h"
#include "constants.h"
#include "var.h"

#include <stdint.h>

typedef struct {
	char name[MAX_LEX_LEN];
	var_t params[MAX_PARAM_LEN];
	uint8_t n_param;
	node_t *ptr;
} func_t;

typedef struct {
	uint16_t len;
	uint16_t size;
	func_t *array;
} funcs_t;

typedef struct {
	value_t *value;
} params_t;

void func_add_param(func_t *func, char *name);
void add_func(funcs_t *funcs, char *name, node_t *ptr);
void funcs_init(funcs_t *funcs);
func_t *get_func(funcs_t *funcs, char *name);
