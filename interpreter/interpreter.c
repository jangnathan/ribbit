#include "interpreter.h"
#include "constants.h"
#include "user_error.h"

#include <stdio.h>

enum STATUS {
	ST_NONE,
	ST_LEX,
	ST_LEX_END,
	ST_END
};

void ctx_init(ctx_t *ctx) {
	interpreter_t preter;
	interpreter_init(&preter);
	ctx->preter = &preter;
	// genesis
	ctx->temp_node = new_node(&preter);
	ctx->tk_i = 0;
	ctx->lex[0] = '\0';
	ctx->status = ST_NONE;
}

void interpreter_init(interpreter *preter) {
	preter->ast_len = 0;
	preter->ast_size = 16;
	preter->ast_array = malloc(sizeof(node_t) * preter->size);	

	funcs_init(preter->funcs);
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
	funcs_t *funcs = ctx->preter->funcs;

	switch (ctx->status) {
		case ST_NONE: {
			if (is_lex(ch)) {
				ctx->status = ST_LEX;
				ctx->i = 0;
			}
		}
		case ST_LEX: {
			if (is_lex(ch)) {
				ctx->lex[ctx->i] = ch;
				if (ctx->i > MAX_LEX_LEN) {
					return user_err("");
				}
				ctx->i++;
			} else if (is_whitespace(ch)) {
				ctx->status = ST_LEX_END;
				ctx->lex[ctx->i] = '\0';
			} else if (ch == '(') {
				ctx->lex[ctx->i] = '\0';
				goto lex_par;
			} else {
				return user_err("unexpected symbol");
			}
		}
		case ST_LEX_END: {
			if (ch == '(') {
			lex_par:
				// if its a function
				func_t *func = get_func(funcs, ctx->lex);
				if (func == NULL) {
					return user_err("function doesnt exist");
				} else {
					ctx->temp_node->type = CALL;
					ctx->temp_node->ptr = func;
				}
			}
		}
		case ST_END: {
		}
	}
	return 1;
}

uint8_t load_file(ctx_t *ctx, FILE *file) {
	char ch;
	while ((ch = fgetc(file)) != EOF) {
		if (!process(ctx, ch)) return 0;
	}
	return 1;
}

uint8_t run_ast(node_t *ast) {
}

// where would it even add code anyway?
// maybe onto the ast..
// would it need a second pointer for the entry point?
// hmm but it also needs to know the next instruction
