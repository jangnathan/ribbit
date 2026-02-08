#include "ast.h"
#include <stdlib.h>

node_t *new_node(ast_t *ast) {
	ast->len++;
	if (ast->len > ast->size) {
		ast->size *= 2;
		ast->array = realloc(ast->array, sizeof(node_t) * ast->size);
	}

	node_t *new = &ast->array[ast->len - 1];
	new->next = 0;
	new->type = END;

	return new;
}

// should it have pointers to the next node instead?
// hmmm
