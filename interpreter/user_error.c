#include "user_error.h"
#include "constants.h"
#include <stdio.h>

uint8_t user_err(char *msg) {
	printf(RED_CONSOLE "\nERROR: %s" WHITE_CONSOLE, msg);
	return 0;
}
