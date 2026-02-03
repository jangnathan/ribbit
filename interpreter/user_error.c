#include "user_error.h"
uint8_t user_err(char *msg) {
	printf("%s", msg);
	return 0;
}
