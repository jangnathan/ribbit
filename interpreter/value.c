#include "value.h"
#include "helpers.h"
#include <math.h>
#include <stdlib.h>

void values_init(values_t *values) {
	values->len = 0;
	values->size = 4;
	values->array = malloc(sizeof(value_t) * values->size);
}
uint16_t new_value(values_t *values) {
	if (values->len >= values->size) {
		values->size *= 2;
		values->array = realloc(values->array, sizeof(value_t) * values->size);
	}

	values->array[values->len].type = UNDEFINED;
	values->len++;

	return values->len - 1;
}

void parse_number(rt_ints_t *rt_ints, value_t *value, char lex[MAX_LEX_LEN], uint8_t len) {
	if (value->type == I32) {
		value->ptr = new_i32(rt_ints);

		int32_t *i32 = &rt_ints->i32s[rt_ints->i32_len - 1];
		for (uint8_t i = 0; i < len; i++) {
			*i32 += char2digit(lex[len - i - 1]) * pow(10, i);
		}
	}else if (value->type == I64) {
		value->ptr = new_i64(rt_ints);

		int64_t *i64 = &rt_ints->i64s[rt_ints->i64_len - 1];
		for (uint8_t i = 0; i < len; i++) {
			*i64 += char2digit(lex[len - i - 1]) * pow(10, i);
		}
	} else if (value->type == FLOAT) {
		uint8_t dist2point;
	}
}

uint8_t numbers_equal(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr) {
	if (l_type == I32 && r_type == I32) {
		return i32_equal_i32(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == I32 && r_type == I64) {
		return i32_equal_i64(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == I64 && r_type == I32) {
		return i32_equal_i64(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == BOOL) {
		return l_ptr == r_ptr;
	}
	if (l_type == I64 && r_type == BOOL) {
		return i64_equal_bool(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == I64) {
		return i64_equal_bool(rt_ints, r_ptr, l_ptr);
	}
	if (l_type == I64 && r_type == BOOL) {
		return i32_equal_bool(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == I64) {
		return i32_equal_bool(rt_ints, r_ptr, l_ptr);
	}
	return 0;
}

uint8_t numbers_less(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr) {
	if (l_type == I32 && r_type == I32) {
		return i32_less_i32(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == I32 && r_type == I64) {
		return i32_less_i64(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == I64 && r_type == I32) {
		return i32_less_i64(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == BOOL) {
		return l_ptr < r_ptr;
	}
	if (l_type == I64 && r_type == BOOL) {
		return i64_less_bool(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == I64) {
		return i64_less_bool(rt_ints, r_ptr, l_ptr);
	}
	if (l_type == I64 && r_type == BOOL) {
		return i32_less_bool(rt_ints, l_ptr, r_ptr);
	}
	if (l_type == BOOL && r_type == I64) {
		return i32_less_bool(rt_ints, r_ptr, l_ptr);
	}
	return 0;
}
