#pragma once

enum NODE_TYPE {
	BLOCK,
	DECLARATION,
	CALL,
	IF,
	FOR_LOOP,

	// exp
	ADD,
	SUB,
	EQUAL,
	NOT_EQUAL
};

typedef struct node {
	enum NODE_TYPE;
	struct node *nodes;
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
