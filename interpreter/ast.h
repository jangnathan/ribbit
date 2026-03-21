#pragma once
#include <stdint.h>

enum NODE_TYPE {
	END,

	BLOCK,
	DECLARATION,
	CALL,
	IF,
	FOR_LOOP,
	END_LOOP,

	VALUE,
	REFERENCE,

	// exp
	ADD,
	SUB,
	PARENTHESIS, // just to group things together
	EQUAL,
	NOT_EQUAL,
	MORE,
	LESS,
	MORE_EQUAL,
	LESS_EQUAL,
};
void log_nodetype(enum NODE_TYPE type);

uint8_t has_end_parenthesis(enum NODE_TYPE type);
uint8_t has_curly_braces(enum NODE_TYPE type);
uint8_t is_statement(enum NODE_TYPE type);
uint8_t should_eval(enum NODE_TYPE type);
uint8_t is_compare(enum NODE_TYPE type);

enum NODE_STATE {
	NS_NONE,
	NS_BACK,
	NS_END,
	NS_END_CALL,
};

typedef struct node {
	uint32_t ptr;
	uint32_t next_id;
	uint32_t parent_id;
	enum NODE_TYPE type;
	enum NODE_STATE state; // bool saying that it exists the current exp
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

uint32_t new_node(ast_t *ast);
