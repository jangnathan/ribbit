#pragma once
#include "func.h"
#include "ast.h"
#include "var.h"
#include "string.h"
#include "number.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
	ast_t ast;

	funcs_t funcs;
	vars_t vars;
	values_t values;
	strings_t strings;
	rt_ints_t rt_ints;
} interpreter_t;

enum STATUS {
	ST_NONE,
	ST_STRING,
	ST_NUMBER,
	ST_LEX,
	ST_LEX_END,
	ST_EQUAL_SYMBOL,
	ST_EQUAL_SYMBOL_LEX,
	ST_END
};

typedef struct {
	interpreter_t *preter;
	node_t *temp_node;
	uint8_t i;
	char lex[MAX_LEX_LEN];
	enum STATUS status;
	char last_ch;
} ctx_t;

void interpreter_init(interpreter_t *preter);
void ctx_init(ctx_t *ctx);
uint8_t load_file(ctx_t *ctx, FILE *file);
void cleanup(interpreter_t *preter);
