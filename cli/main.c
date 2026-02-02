#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, int *argv[]) {
	if (argv[1] == NULL) {
		printf("needs argument\n");
		return 1;
	}

	char *path = argv[1];

	interpreter_t preter;
	interpreter_init(&preter);

	FILE *file;
	file = fopen(path, "r");
	if (file == NULL) {
		printf("file doesnt exist\n");
		return 1;
	}
	uint8_t success = load_file(&preter, file);
	fclose(file);
	if (!success) return 1;

	run_ast(&preter);

	return 0;
}
