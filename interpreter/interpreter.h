#pragma once
#include "funcs.h"
#include "ast.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct {
	node_t *ast_array;
	uint32_t ast_len;
	uint32_t ast_size;

	func_t *funcs;
} interpreter_t;

typedef struct {
	interpreter_t *preter;
	node_t *temp_node;
	uint8_t i;
	char lex[MAX_LEX_LEN];
	enum STATUS status;
} ctx_t;

void ctx_init(ctx_t *ctx);
uint8_t load_file(node_t *node, char *path);
uint8_t run_ast(node_t *ast);
