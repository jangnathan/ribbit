#include "ast.h"

node_t *new_node(interpreter *preter) {
	preter->ast_len++;
	if (preter->ast_len > preter->ast_size) {
		preter->ast_size *= 2;
		preter->ast_array = realloc(preter->ast_array, sizeof(node_t) * preter->ast_size);
	}

	node_t *node = preter->ast_array[preter->ast_len - 1];
	node->nodes_len = 0;
	node->nodes_size = 1;
	node->nodes = malloc(sizeof(node_t));

	return node;
}

// should it have pointers to the next node instead?
// hmmm
node_t *new_child(interpreter *preter, node_t *parent) {
	parent->nodes_len++;
	if (parent->nodes_len > parent->nodes_size) {
		parent->nodes_size *= 2;
		parent->nodes = realloc(parent_nodes, sizeof(node_t) * preter->ast_size);
	}
	node_t *node = new_node(preter);
}
