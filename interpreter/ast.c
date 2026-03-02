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
	printf("%s", nodetype_string[(uint8_t)type]);
}

uint8_t should_eval(enum NODE_TYPE type) {
	if (type >= ADD) {
		return 1;
	}
	return 0;
}

uint32_t new_node(ast_t *ast) {
	node_t *new = &ast->array[ast->len];
	ast->len++;

	new->type = BLOCK;
	new->next_id = 0;
	new->state = NS_NONE;
	new->type = END;

	if (ast->len >= ast->size) {
		ast->size *= 2;
		ast->array = realloc(ast->array, sizeof(node_t) * ast->size);
	}

	return ast->len - 1;
}

// should it have pointers to the next node instead?
// hmmm
