#pragma once
#include "interpreter.h"
#include "ast.h"
#include "func.h"
#include "var.h"

#include <stdint.h>

uint8_t run(interpreter_t *preter);
uint8_t print_chain(interpreter_t *preter);
