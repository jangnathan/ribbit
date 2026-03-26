#pragma once
#include <stdint.h>
#include "number.h"

enum DATATYPE {
	UNDEFINED,
	STRING,
	I32,
	I64,
	FLOAT,
	BOOL
};

typedef struct {
	enum DATATYPE type;
	uint16_t ptr;
} value_t;

typedef struct {
	value_t *array;
	uint16_t len;
	uint16_t size;
} values_t;

void values_init(values_t *values);
uint16_t new_value(values_t *values);

void parse_number(rt_ints_t *rt_ints, value_t *value, char lex[MAX_LEX_LEN], uint8_t len);

uint8_t numbers_equal(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr);
uint8_t numbers_less(rt_ints_t *rt_ints, enum DATATYPE l_type, uint16_t l_ptr, enum DATATYPE r_type, uint16_t r_ptr);
uint8_t is_num_type(enum DATATYPE type);
