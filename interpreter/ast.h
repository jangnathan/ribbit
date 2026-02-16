#pragma once
#include <stdint.h>

enum NODE_TYPE {
	END,

	BLOCK,
	DECLARATION,
	CALL,
	IF,
	FOR_LOOP,

	// literals
	LITERAL,
	REFERENCE,

	// exp
	ADD,
	SUB,
	EQUAL,
	NOT_EQUAL,
	PARENTHESIS // just to group things together
};
void log_node_type(enum NODE_TYPE type);

typedef struct node {
	enum NODE_TYPE type;
	void *ptr;
	struct node *next;
	struct node *parent;
	uint8_t back; // bool saying that it exists the current exp
} node_t;
// you do go back??
// how do you know if something should be calculated first?
// if its math..., if its math then you evaluate

typedef struct {
	node_t *array;
	uint32_t len;
	uint32_t size;
} ast_t;
/*
var text = "helo"
print(text)

if text == "hello" {
 // block
}

// also have loops

should it be a poitnerk
*/

/*
BLOCK {
DECLARE var 10,
IF var 10 BLOCK {
PRINT var
},
PRINT "end"
}

soo....
declare and print have same expressions
IF also has an expression
*/

node_t *new_node(ast_t *ast);
node_t *new_child(ast_t *ast, node_t *parent);
