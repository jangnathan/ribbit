#pragma once
#include <stdint.h>

enum DATATYPE {
	STRING,
	INT,
	FLOAT
};

typedef struct {
	enum DATATYPE type;
	void *ptr;
} value_t;

typedef struct {
	value_t *array;
	uint32_t len;
	uint32_t size;
} values_t;

void values_init(values_t *values);
value_t *new_value(values_t *values);
