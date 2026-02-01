#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, int *argv[]) {
	if (argv[1] == NULL) {
		printf("needs argument\n");
		return 1;
	}

	char *path = argv[1];
	FILE *file;
	file = fopen(path, "r");
	if (file == NULL) {
		printf("file doesnt exist\n");
		return 1;
	}

	node_t ast;
	load_file(&ast, path);
	run_ast(&ast);

	fclose(file);
	return 0;
}
