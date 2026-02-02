#include "interpreter.h"
#include "constants.h"

#include <stdio.h>

enum STATUS {
	ST_NONE,
	ST_LEX,
	ST_END
};

typedef struct {
	uint8_t idx;
	char lex[MAX_LEX_LEN];
	enum STATUS status;
} ctx_t;

uint8_t interpreter_init(interperter_t *preter) {
	preter->ast_len = 0;
	preter->ast_size = 256
	preter->ast_array = malloc(sizeof(node_t) * preter->size);
}

bool is_whitespace(char ch) {
	if (ch == '\n' || ch == ' ' || ch == '\t') {
		return true;
	}
	return false;
}
bool is_lex(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) return true;
	if (chn >= 64 && chn <= 90) return true;
	if (chn >= 97 && chn <= 122) return true;
	if (ch == '_') return true;
	return false;
}
bool is_num(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) {
		return true;
	}
	return false;
}
uint8_t char2digit(char ch) {
	uint8_t chn = (uint8_t)ch;
	return chn - 48;
}
bool is_operator(char ch) {
	if (ch=='+'||ch=='-'||ch=='/'||ch=='*'||
		ch=='<'||ch=='>') {
		return true;
	}
	return false;
}

// it needs to get the instruction
// it needs to make a tree
// how to do that?
// it does so by going through the file
// it finds a statement, and if theres stuff inside the parameters
// there are new nodes??
// like setting a variable...
// should there be a symbol table for each scope?
// no thats stupid, everything will just be global, its simpler, since this is just an ast
// Have it be a hash table
// Current node -> if it reads something like my_var = 1
// then it creates a DECLARATION node
uint8_t process(ctx_t *ctx, char ch) {
	switch (ctx->status) {
		case ST_NONE: {
			if (is_lex(ch)) {
				ctx->status = ST_LEX;
			}
		}
		case ST_LEX: {
			if (is_whitespace(ch)) {
				ctx->status = ST_END;
			}
		}
	}
	return 1;
}

uint8_t load_file(node_t *node, FILE *file) {
	ctx_t ctx;
	ctx->tk_idx = 0;
	ctx->lex[0] = '\0';
	ctx->status = ST_NONE;

	char ch;
	while ((ch = fgetc(file)) != EOF) {
		if (!process(&ctx, ch)) return 0;
	}
	return 1;
}

uint8_t run_ast(node_t *ast) {
}

// where would it even add code anyway?
// maybe onto the ast..
// would it need a second pointer for the entry point?
// hmm but it also needs to know the next instruction
