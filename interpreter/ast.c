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
	"VALUE",
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

uint32_t new_node(ast_t *ast) {
	if (ast->len >= ast->size) {
		ast->size *= 2;
		ast->array = realloc(ast->array, sizeof(node_t) * ast->size);
	}

	node_t *new = &ast->array[ast->len];
	ast->len++;

	new->next = 0;
	new->back = 0;
	new->type = END;

	return ast->len - 1;
}

// should it have pointers to the next node instead?
// hmmm

