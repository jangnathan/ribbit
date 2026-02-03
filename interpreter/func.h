#pragma once
typedef struct {
	char name[MAX_LEX_LEN];
	node_t *ptr;
} func_t;

void add_func(func_t *funcs, char *name, node_t *ptr);
void funcs_init(func_t *funcs);
func_t *get_func(func_t *funcs, char *name);
