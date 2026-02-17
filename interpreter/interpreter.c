#include "interpreter.h"
#include "constants.h"
#include "user_error.h"
#include "value.h"

#include <stdio.h>

void interpreter_init(interpreter_t *preter) {
	#ifdef DEBUG
	printf("init interpreter start\n");
	#endif

	preter->ast.len = 0;
	preter->ast.size = 16;
	preter->ast.array = malloc(sizeof(node_t) * preter->ast.size);	

	values_init(&preter->values);
	funcs_init(&preter->funcs);
	strings_init(&preter->strings);
}

void ctx_init(ctx_t *ctx) {
	#ifdef DEBUG
	printf("init ctx start\n");
	#endif

	// genesis
	new_node(&ctx->preter->ast);
	ctx->temp_node = &ctx->preter->ast.array[0];
	ctx->temp_node->type = BLOCK;
	ctx->i = 0;
	ctx->lex[0] = '\0';
	ctx->status = ST_NONE;

	#ifdef DEBUG
	printf("init ctx end\n");
	#endif
}

uint8_t is_whitespace(char ch) {
	if (ch == '\n' || ch == ' ' || ch == '\t') {
		return 1;
	}
	return 0;
}
uint8_t is_lex(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) return 1;
	if (chn >= 64 && chn <= 90) return 1;
	if (chn >= 97 && chn <= 122) return 1;
	if (ch == '_') return 1;
	return 0;
}
uint8_t is_num(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) {
		return 1;
	}
	return 0;
}
uint8_t char2digit(char ch) {
	uint8_t chn = (uint8_t)ch;
	return chn - 48;
}
uint8_t is_operator(char ch) {
	if (ch=='+'||ch=='-'||ch=='/'||ch=='*'||
		ch=='<'||ch=='>') {
		return 1;
	}
	return 0;
}

node_t *append_child(ast_t *ast, node_t *temp_node) {
	node_t *new = &ast->array[new_node(ast)];
	new->parent = temp_node;
	temp_node->next = new;
	return new;
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
	interpreter_t *preter = ctx->preter;
	funcs_t *funcs = &preter->funcs;
	vars_t *vars = &preter->vars;
	values_t *values = &preter->values;
	strings_t *strings = &preter->strings;
	ast_t *ast = &preter->ast;

#ifdef DEBUG
	printf("%c", ch);
	fflush(stdout);
#endif

	switch (ctx->status) {
		case ST_NONE: {
			ctx->i = 0;
			if (is_lex(ch)) {
				ctx->status = ST_LEX;
				goto lex;
			} else if (ch == '"') {
				ctx->status = ST_STRING;

				ctx->temp_node->type = LITERAL;
				uint16_t id = new_value(values);
				value_t *value = &values->array[id];
				value->type = STRING;
				ctx->temp_node->ptr = id;

				id = new_string(strings);
				value->ptr = id;
			}
			break;
		}
		case ST_LEX: {
			if (is_lex(ch)) {
			lex:
				ctx->lex[ctx->i] = ch;
				if (ctx->i > MAX_LEX_LEN) {
					return user_err("lex is too long");
				}
				ctx->i++;
			} else if (is_whitespace(ch)) {
				ctx->status = ST_LEX_END;
				ctx->lex[ctx->i] = '\0';
			} else if (ch == '(') {
				ctx->lex[ctx->i] = '\0';
				goto lex_par;
			} else if (ch == '=') {
				ctx->lex[ctx->i] = '\0';
				goto lex_equal;
			} else {
				return user_err("unexpected symbol");
			}
			break;
		}
		case ST_STRING: {
			if (ch == '"') {
				ctx->status = ST_END;
			} else {
				value_t value = values->array[ctx->temp_node->ptr];
				string_t *str = &strings->array[value.ptr];
				add2string(str, ch);
			}
			break;
		}
		case ST_LEX_END: {
			if (ch == '(') {
			lex_par:
				ctx->temp_node->type = CALL;

				// if its a function
				int32_t func_id = get_func(funcs, ctx->lex);
				func_t *func = &funcs->array[func_id];
				if (func_id == -1) {
					return user_err("function doesnt exist");
				}
				ctx->temp_node->ptr = func_id;
				if (func->n_param > 0) {
					if (func->n_param == 1) {
						ctx->temp_node = append_child(ast, ctx->temp_node);
					} else {
						ctx->temp_node = append_child(ast, ctx->temp_node);
						ctx->temp_node->type = PARENTHESIS;

						ctx->temp_node = append_child(ast, ctx->temp_node);
					}
				}

				ctx->status = ST_NONE;
			}
			break;
		}
		case ST_END: {
			if (ch == '=') {
			lex_equal:
				ctx->temp_node->type = DECLARATION;
				int32_t var = get_var(vars, ctx->lex);
				if (var == -1) {
					var = new_var(vars, ctx->lex);
				}
				ctx->temp_node->ptr = var;

				ctx->status = ST_NONE;
			} else if (ch == '+') {
				node_t *operator = &ast->array[new_node(ast)];
				ctx->temp_node->parent->next = operator;
				operator->parent = ctx->temp_node->parent;
				operator->next = ctx->temp_node;
				ctx->temp_node->parent = operator;

				node_t *og_node = ctx->temp_node;
				ctx->temp_node = &ast->array[new_node(ast)];
				ctx->temp_node->parent = operator;
				og_node->next = ctx->temp_node;

				ctx->status = ST_NONE;
			} else if (ch == ')') {
				if (ctx->temp_node->parent->type == CALL) {
					ctx->temp_node->back = 1;

					node_t *og_node = ctx->temp_node;
					ctx->temp_node = &ast->array[new_node(ast)];
					ctx->temp_node->parent = og_node->parent->parent;
					og_node->next = ctx->temp_node;

					ctx->status = ST_NONE;
					break;
				}

				return user_err("unexpected ')'");
			}
			break;
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
// where would it even add code anyway?
// maybe onto the ast..
// would it need a second pointer for the entry point?
// hmm but it also needs to know the next instruction
