#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "ast.h"

typedef struct {
	node_t *ast;

	uint8_t tok_idx;
	char tok[MAX_TOK_LEN];
} interpreter_t;

uint8_t load_file(node_t *node, char *path);
uint8_t run_ast(node_t *node);
