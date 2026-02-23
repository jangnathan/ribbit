#pragma once
#include <stdint.h>

enum DATATYPE {
	UNDEFINED,
	STRING,
	I32,
	I64,
	FLOAT
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
