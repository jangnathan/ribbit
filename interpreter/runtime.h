#pragma once
#include "interpreter.h"
#include "ast.h"
#include "func.h"
#include "var.h"

#include <stdint.h>

uint8_t should_eval(enum NODE_TYPE type);
uint8_t run_ast(ast_t *ast);
uint8_t print_ast(ast_t *ast);
uint8_t print_chain(ast_t *ast);
