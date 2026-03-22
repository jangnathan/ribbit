#pragma once
#include "constants.h"
#include <stdint.h>

typedef struct {
	int32_t *i32s;
	int16_t i32_len;
	int16_t i32_size;

	// queues for spare memory bits
	int16_t i32_q[32];
	int8_t i32_q_len;

	int64_t *i64s;
	int16_t i64_len;
	int16_t i64_size;
	int16_t i64_q[32];
	int8_t i64_q_len;
} rt_ints_t;

void rt_ints_init(rt_ints_t *rt_ints);

void delete_i32(rt_ints_t *rt_ints, uint16_t id);
void delete_i64(rt_ints_t *rt_ints, uint16_t id);

uint16_t new_i32(rt_ints_t *rt_ints);
uint16_t new_i64(rt_ints_t *rt_ints);

uint8_t i32_equal_i32(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i32_equal_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i64_equal_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i64_equal_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r);
uint8_t i32_equal_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r);
uint8_t i32_less_i32(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i32_less_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i64_less_i64(rt_ints_t *rt_ints, uint16_t l, uint16_t r);
uint8_t i64_less_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r);
uint8_t i32_less_bool(rt_ints_t *rt_ints, uint16_t l, uint8_t r);
