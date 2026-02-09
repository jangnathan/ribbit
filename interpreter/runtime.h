#pragma once
#include "interpreter.h"
#include "ast.h"
#include "func.h"
#include "var.h"

#include <stdint.h>

uint8_t run_ast(ast_t *ast);
