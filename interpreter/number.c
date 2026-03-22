#include "number.h"
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

