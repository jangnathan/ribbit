#include "number.h"
#include "helpers.h"

#include <math.h>
#include <stdlib.h>

void rt_ints_init(rt_ints_t *rt_ints) {
	rt_ints->i32_len = 0;
	rt_ints->i32_size = 4;
	rt_ints->i32s = malloc(sizeof(int32_t) * rt_ints->i32_size);
	rt_ints->i64_len = 0;
	rt_ints->i64_size = 4;
	rt_ints->i64s = malloc(sizeof(int64_t) * rt_ints->i64_size);
}

void parse_number(rt_ints_t *rt_ints, value_t *value, char lex[MAX_LEX_LEN], uint8_t len) {
	if (value->type == I64) {
		value->ptr = rt_ints->i64_len;
		rt_ints->i64s[rt_ints->i64_len] = 0;
		rt_ints->i64_len++;
		if (rt_ints->i64_len >= rt_ints->i64_size) {
			rt_ints->i64_size *= 2;
			rt_ints->i64s = realloc(rt_ints->i64s, sizeof(int64_t) * rt_ints->i64_size);
		}

		int64_t *i64 = &rt_ints->i64s[rt_ints->i64_len - 1];
		for (uint8_t i = 0; i < len; i++) {
			*i64 += char2digit(lex[len - i - 1]) * pow(10, i);
		}
	} else if (value->type == I32) {
		value->ptr = rt_ints->i32_len;
		rt_ints->i32s[rt_ints->i32_len] = 0;
		rt_ints->i32_len++;
		if (rt_ints->i32_len >= rt_ints->i32_size) {
			rt_ints->i32_size *= 2;
			rt_ints->i32s = realloc(rt_ints->i32s, sizeof(int32_t) * rt_ints->i32_size);
		}
		int32_t *i32 = &rt_ints->i32s[rt_ints->i32_len - 1];
		for (uint8_t i = 0; i < len; i++) {
			*i32 += char2digit(lex[len - i - 1]) * pow(10, i);
		}
	} else if (value->type == FLOAT) {
		uint8_t dist2point;
	}
}
