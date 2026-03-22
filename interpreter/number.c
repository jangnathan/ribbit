#include "number.h"
#include "helpers.h"

#include <math.h>
#include <stdlib.h>

void rt_ints_init(rt_ints_t *rt_ints) {
	rt_ints->i32_len = 0;
	rt_ints->i32_size = 4;
	rt_ints->i32s = malloc(sizeof(int32_t) * rt_ints->i32_size);
	rt_ints->i32_q_len = 0;
	rt_ints->i64_len = 0;
	rt_ints->i64_size = 4;
	rt_ints->i64s = malloc(sizeof(int64_t) * rt_ints->i64_size);
	rt_ints->i64_q_len = 0;
}

// simply add to the queue
void delete_i32(rt_ints_t *rt_ints, uint16_t id) {
	rt_ints->i32_q[rt_ints->i32_q_len] = id;
	rt_ints->i32_q_len++;
}
void delete_i64(rt_ints_t *rt_ints, uint16_t id) {
	rt_ints->i64_q[rt_ints->i64_q_len] = id;
	rt_ints->i64_q_len++;
}

uint16_t new_i32(rt_ints_t *rt_ints) {
	uint16_t id;
	if (rt_ints->i32_q_len > 0) {
		rt_ints->i32_q_len--;
		id = rt_ints->i32_q[rt_ints->i32_q_len];
		rt_ints->i32s[id] = 0;
	} else {
		id = rt_ints->i32_len;
		rt_ints->i32s[id] = 0;
		rt_ints->i32_len++;
	}
	if (rt_ints->i32_len >= rt_ints->i32_size) {
		rt_ints->i32_size *= 2;
		rt_ints->i32s = realloc(rt_ints->i32s, sizeof(int32_t) * rt_ints->i32_size);
	}

	return id;
}
uint16_t new_i64(rt_ints_t *rt_ints) {
	uint16_t id;
	if (rt_ints->i64_q_len > 0) {
		rt_ints->i64_q_len--;
		id = rt_ints->i64_q[rt_ints->i64_q_len];
		rt_ints->i32s[id] = 0;
	} else {
		id = rt_ints->i64_len;
		rt_ints->i64s[id] = 0;
		rt_ints->i64_len++;
	}
	if (rt_ints->i64_len >= rt_ints->i64_size) {
		rt_ints->i64_size *= 2;
		rt_ints->i64s = realloc(rt_ints->i64s, sizeof(int64_t) * rt_ints->i64_size);
	}
	return id;
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

uint8_t i32_equal_i32(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i32s[l] == rt_ints->i32s[r];
}
uint8_t i32_equal_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i32s[l] == rt_ints->i64s[r];
}
uint8_t i64_equal_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i64s[l] == rt_ints->i64s[r];
}
uint8_t i64_equal_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r) {
	return rt_ints->i64s[l] == r;
}
uint8_t i32_equal_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r) {
	return rt_ints->i32s[l] == r;
}

uint8_t i32_less_i32(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i32s[l] < rt_ints->i32s[r];
}
uint8_t i32_less_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i32s[l] < rt_ints->i64s[r];
}
uint8_t i64_less_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r) {
	return rt_ints->i64s[l] < rt_ints->i64s[r];
}
uint8_t i64_less_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r) {
	return rt_ints->i64s[l] < r;
}
uint8_t i32_less_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r) {
	return rt_ints->i32s[l] < r;
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
