#include "ast.h"

node_t *new_node(ast_t *ast) {
	ast->len++;
	if (ast->len > ast->size) {
		ast->size *= 2;
		ast->array = realloc(ast->array, sizeof(node_t) * ast->size);
	}

	return ast->array[ast->len - 1];
}

// should it have pointers to the next node instead?
// hmmm
