#pragma once
#include <stdint.h>

enum NODE_TYPE {
	BLOCK,
	DECLARATION,
	CALL,
	IF,
	FOR_LOOP,

	STRING,
	NUMBER,
	REFERENCE,

	// exp
	ADD,
	SUB,
	EQUAL,
	NOT_EQUAL,
	PARENTHESIS,

	END
};

typedef struct node {
	enum NODE_TYPE type;
	void *ptr;
	struct node *next;
	struct node *parent;
	struct node *child;
} node_t;

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
