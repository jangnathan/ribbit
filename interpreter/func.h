#pragma once
typedef struct {
	char name[MAX_LEX_LEN];
	node_t *ptr;
} func_t;

void add_func(funcs_t *funcs, char *name, node_t *ptr);
void funcs_init(funcs_t *funcs);
func_t *get_func(funcs_t *funcs, char *name);
