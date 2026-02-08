#pragma once
#include "func.h"
#include "ast.h"
#include "var.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
	ast_t ast;

	funcs_t funcs;
	vars_t vars;
} interpreter_t;

enum STATUS {
	ST_NONE,
	ST_STRING,
	ST_LEX,
	ST_LEX_END,
	ST_END
};

typedef struct {
	interpreter_t *preter;
	node_t *temp_node;
	uint8_t i;
	char lex[MAX_LEX_LEN];
	enum STATUS status;
} ctx_t;

void interpreter_init(interpreter_t *preter);
void ctx_init(ctx_t *ctx);
uint8_t load_file(ctx_t *ctx, FILE *file);
uint8_t run_ast(ast_t *ast);
uint8_t print_ast(ast_t *ast);
