#pragma once
enum NODE_TYPE {
	BLOCK,
	DECLARATION,
	CALL,
	IF,
	FOR_LOOP,

	STRING,
	NUMBER,
	REFERENCE

	// exp
	ADD,
	SUB,
	EQUAL,
	NOT_EQUAL
};

typedef struct node {
	enum NODE_TYPE type;
	void *ptr;
	struct node *nodes;
	uint16_t nodes_len;
	uint16_t nodes_size;

	struct node *parent;
} node_t;
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

node_t *new_node(interpreter *preter);
node_t *new_child(interpreter *preter, node_t *parent);
