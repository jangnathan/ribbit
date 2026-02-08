#pragma once
#include "ast.h"
#include "constants.h"

#include <stdint.h>

typedef struct {
	char name[MAX_LEX_LEN];
	node_t *ptr;
} func_t;

typedef struct {
	uint16_t len;
	uint16_t size;
	func_t *array;
} funcs_t;

void add_func(funcs_t *funcs, char *name, node_t *ptr);
void funcs_init(funcs_t *funcs);
func_t *get_func(funcs_t *funcs, char *name);
