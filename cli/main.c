#include "../interpreter/interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	if (argv[1] == NULL) {
		printf("needs argument\n");
		return 1;
	}
	char *command = argv[1];

	if (argv[2] == NULL) {
		printf("needs two arguments");
		return 1;
	}
	char *path = argv[2];
	printf("%s\n", path);

	interpreter_t preter;
	interpreter_init(&preter);
	ctx_t ctx;
	ctx.preter = &preter;
	ctx_init(&ctx);

	FILE *file;
	file = fopen(path, "r");
	if (file == NULL) {
		printf("file doesnt exist\n");
		return 1;
	}
	uint8_t success = load_file(&ctx, file);
	fclose(file);
	printf("File loaded\n");
	if (!success) return 1;
	printf("AST WORKS\n");

	if (strcmp(command, "run") == 0) {
		run_ast(&ctx.preter->ast);
	} else if (strcmp(command, "print") == 0) {
		print_ast(&ctx.preter->ast);
	}

	return 0;
}
