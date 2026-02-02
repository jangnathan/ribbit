#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "ast.h"

typedef struct {
	node_t *ast_array;
	uint32_t ast_len;
	uint32_t ast_size;
} interpreter_t;

uint8_t interpreter_init(interperter_t *preter)
uint8_t load_file(node_t *node, char *path);
uint8_t run_ast(node_t *ast);
