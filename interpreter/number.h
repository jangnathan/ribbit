#pragma once
#include "value.h"
#include "constants.h"

typedef struct {
	int32_t *i32s;
	int16_t i32_len;
	int16_t i32_size;
	int64_t *i64s;
	int16_t i64_len;
	int16_t i64_size;
} rt_ints_t;

void rt_ints_init(rt_ints_t *rt_ints);
void parse_number(rt_ints_t *rt_ints, value_t *value, char lex[MAX_LEX_LEN], uint8_t len);
