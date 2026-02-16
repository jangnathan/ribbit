#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

char *nodetype_string[] = {
	"END",
	"BLOCK",
	"DECLARATION",
	"CALL",
	"IF",
	"FOR_LOOP",
	"LITERAL",
	"REFERENCE",
	"ADD",
	"SUB",
	"EQUAL",
	"NOT_EQUAL",
	"PARENTHESIS"
};
void log_nodetype(enum NODE_TYPE type) {
	printf("%s\n", nodetype_string[(uint8_t)type]);
}

node_t *new_node(ast_t *ast) {
	node_t *new = &ast->array[ast->len];

	ast->len++;
	if (ast->len > ast->size) {
		ast->size *= 2;
		ast->array = realloc(ast->array, sizeof(node_t) * ast->size);
	}
	new->next = 0;
	new->back = 0;
	new->type = END;

	return new;
}

// should it have pointers to the next node instead?
// hmmm

