#pragma once
#include "value.h"
#include "constants.h"

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

void parse_number(rt_ints_t *rt_ints, value_t *value, char lex[MAX_LEX_LEN], uint8_t len);

uint8_t numbers_equal(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr);
uint8_t numbers_less(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr);
