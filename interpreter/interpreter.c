#include "interpreter.h"
#include "constants.h"
#include "user_error.h"
#include "helpers.h"
#include "value.h"
#include <stdio.h>
#include <string.h>

node_t *append_child(ast_t *ast) {
	uint32_t temp_id = ast->len - 1;
	node_t *temp_node = &ast->array[temp_id];
	uint32_t new_id = new_node(ast);

	node_t *new = &ast->array[new_id];
	new->parent_id = temp_id;
	temp_node->next_id = new_id;
	return new;
}

void interpreter_init(interpreter_t *preter) {
	#ifdef DEBUG
	printf("init interpreter start\n");
	#endif

	preter->ast.len = 0;
	preter->ast.size = 16;
	preter->ast.array = malloc(sizeof(node_t) * preter->ast.size);	

	vars_init(&preter->vars);
	values_init(&preter->values);
	funcs_init(&preter->funcs);
	strings_init(&preter->strings);
	rt_ints_init(&preter->rt_ints);
}

void cleanup(interpreter_t *preter) {
	free(preter->ast.array);
	free(preter->vars.array);
	free(preter->values.array);
	free(preter->funcs.array);
	for (uint16_t i = 0; i < preter->strings.len; i++) {
		free(preter->strings.array[i].array);
	}
	free(preter->strings.array);
	free(preter->rt_ints.i32s);
	free(preter->rt_ints.i64s);
}

void ctx_init(ctx_t *ctx) {
	#ifdef DEBUG
	printf("init ctx start\n");
	#endif

	ast_t *ast = &ctx->preter->ast;

	// genesis
	ctx->temp_node = &ast->array[new_node(ast)];
	ctx->i = 0;
	ctx->lex[0] = '\0';
	ctx->status = ST_NONE;

	#ifdef DEBUG
	printf("init ctx end\n");
	#endif
}

uint8_t lex_handle_ref(ctx_t *ctx, char ch) {
	if (!is_ender(ch)) {
		return user_err("unexpected character");
	}

	if (strcmp(ctx->lex, "true") == 0) {
		ctx->temp_node->type = VALUE;
		uint16_t value_id = new_value(&ctx->preter->values);
		ctx->temp_node->ptr = value_id;
		ctx->preter->values.array[value_id].type = BOOL;
		ctx->preter->values.array[value_id].ptr = 1;
		return 1;
	} else if (strcmp(ctx->lex, "false") == 0) {
		ctx->temp_node->type = VALUE;
		uint16_t value_id = new_value(&ctx->preter->values);
		ctx->temp_node->ptr = value_id;
		ctx->preter->values.array[value_id].type = BOOL;
		ctx->preter->values.array[value_id].ptr = 0;
		return 1;
	}

	vars_t *vars = &ctx->preter->vars;

	int32_t var_id = get_var(vars, ctx->lex);
	if (var_id == -1) {
		printf("%s\n", ctx->lex);
		return user_err("variable doesnt exist");
	}

	ctx->temp_node->type = REFERENCE;
	ctx->temp_node->ptr = var_id;
	return 1;
}

uint8_t is_part_of_type(ast_t *ast, node_t *node, enum NODE_TYPE type) {
	node_t parent = ast->array[node->parent_id];

	if (should_eval(parent.type)) {
		parent = ast->array[parent.parent_id];
	}
	return parent.type == type;
}
enum NODE_TYPE get_higher_id(ast_t *ast, node_t *node) {
	node_t parent = ast->array[node->parent_id];

	if (should_eval(parent.type)) {
		return parent.parent_id;
	}
	return node->parent_id;
}

void build_equal_node(ctx_t *ctx) {
}
void build_add_node(ctx_t *ctx) {
	interpreter_t *preter = ctx->preter;
	ast_t *ast = &preter->ast;
	values_t *values = &preter->values;

	node_t *parent = &ast->array[ctx->temp_node->parent_id];
	uint32_t og_id = ast->len - 1;

	if (should_eval(parent->type) && parent->type != PARENTHESIS) {
		uint32_t oper_id = new_node(ast);
		node_t *oper = &ast->array[oper_id];
		node_t *grandpa = &ast->array[parent->parent_id];

		grandpa->next_id = oper_id;
		oper->next_id = ctx->temp_node->parent_id;
		oper->parent_id = parent->parent_id;
		parent->parent_id = oper_id;

		oper->type = ADD;
		oper->ptr = new_value(values);

		uint32_t temp_id = new_node(ast);
		ctx->temp_node->next_id = temp_id;

		ctx->temp_node = &ast->array[temp_id];
		ctx->temp_node->parent_id = oper_id;
		ctx->temp_node->state = NS_BACK;
	} else {
		uint32_t copy_id = new_node(ast);
		node_t *copy = &ast->array[copy_id];

		copy->type = ctx->temp_node->type;
		copy->ptr = ctx->temp_node->ptr;
		copy->parent_id = og_id;
		// next id is set
		copy->state = 0;

		ctx->temp_node->type = ADD;
		ctx->temp_node->ptr = new_value(values);
		// parent is already set
		ctx->temp_node->next_id = copy_id;
		ctx->temp_node->state = 0;

		uint32_t temp_id = new_node(ast);
		ctx->temp_node = &ast->array[temp_id];
		ctx->temp_node->parent_id = og_id;
		copy->next_id = temp_id;
		ctx->temp_node->state = NS_BACK;
	}
}

uint8_t process(ctx_t *ctx, char ch) {
	interpreter_t *preter = ctx->preter;
	funcs_t *funcs = &preter->funcs;
	vars_t *vars = &preter->vars;
	values_t *values = &preter->values;
	strings_t *strings = &preter->strings;
	ast_t *ast = &preter->ast;

	switch (ctx->status) {
		// it means the previous symbol was an equal sign
		case ST_EQUAL_SYMBOL_LEX: {
			if (ch == '=') {
				build_equal_node(ctx);
				break;
			} else {
				ctx->temp_node->type = DECLARATION;

				int32_t var_id = get_var(vars, ctx->lex);
				if (var_id == -1) {
					var_id = new_var(vars, ctx->lex);
				}
				ctx->temp_node->ptr = var_id;

				ctx->temp_node = append_child(ast);

				ctx->status = ST_NONE;
				goto st_none;
			}
			break;
		}
		case ST_EQUAL_SYMBOL: {
			if (ch == '=') {
				build_equal_node(ctx);
			} else {
				return user_err("unexpected = symbol");
			}
		}
		case ST_NONE: {
		st_none:
			ctx->i = 0;
			if (is_lex(ch) && !is_num(ch)) {
				ctx->status = ST_LEX;
				goto lex;
			} else if (ch == '"') {
				ctx->status = ST_STRING;

				ctx->temp_node->type = VALUE;
				uint16_t id = new_value(values);
				value_t *value = &values->array[id];
				value->type = STRING;
				ctx->temp_node->ptr = id;

				id = new_string(strings);
				value->ptr = id;
			} else if (is_num(ch) || ch == '-') {
				ctx->status = ST_NUMBER;

				ctx->temp_node->type = VALUE;
				uint16_t id = new_value(values);
				value_t *value = &values->array[id];
				value->type = I32;
				ctx->temp_node->ptr = id;

				if (ch == '-') {
					ctx->lex[0] = '-';
					ctx->i = 1;
				}

				// create nums later
				goto st_int;
			}
			break;
		}
		case ST_NUMBER: {
			if (is_num(ch)) {
			st_int:
				ctx->lex[ctx->i] = ch;
				ctx->i++;
			} else {
				value_t *value = &values->array[ctx->temp_node->ptr];
				if (ctx->i >= 9) {
					value->type = I64;
				}
				if (ch == '.') {
					if (value->type == I64 || value->type == I32) {
						value->type = FLOAT;
					} else {
						return user_err("cannot add extra '.' to number");
					}
					break;
				}
				parse_number(&preter->rt_ints, value, ctx->lex, ctx->i);

				if (is_whitespace(ch)) {
					ctx->status = ST_END;
				} else {
					goto st_end;
				}
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
			} else {
				ctx->lex[ctx->i] = '\0';

				if (is_whitespace(ch)) {
					ctx->status = ST_LEX_END;
				} else if (ch == '(') {
					goto lex_par;
				} else if (ch == '=') {
					ctx->status = ST_EQUAL_SYMBOL_LEX;
				} else {
					// could be just a variable
					goto lex_else;
				}
			}
			break;
		}
		case ST_STRING: {
			value_t value = values->array[ctx->temp_node->ptr];
			string_t *str = &strings->array[value.ptr];
			if (ch == '"') {
				ctx->status = ST_END;
				str->array[str->len] = '\0';
			} else {
				add_char2string_w_id(str, ch);
			}
			break;
		}
		case ST_LEX_END: {
			if (ch == '(') {
			lex_par:
				ctx->status = ST_NONE;

				if (strcmp(ctx->lex, "if") == 0) {
					ctx->temp_node->type = IF;

					// the condition
					// if false, then jump to id in ptr
					ctx->temp_node->ptr = new_node(ast);
					ctx->temp_node = &ast->array[ctx->temp_node->ptr];
					break;
				}

				ctx->temp_node->type = CALL;

				// if its a function
				int32_t func_id = get_func(funcs, ctx->lex);
				if (func_id == -1) return user_err("function doesnt exist");
				ctx->temp_node->ptr = func_id;

				func_t func = funcs->array[func_id];
				if (func.n_param > 0) {
					if (func.n_param == 1) {
						ctx->temp_node = append_child(ast);
					} else {
						ctx->temp_node = append_child(ast);
						ctx->temp_node->type = PARENTHESIS;

						ctx->temp_node = append_child(ast);
					}
				}
			} else if (ch == '=') {
				ctx->status = ST_EQUAL_SYMBOL_LEX;
			} else if (is_whitespace(ch)) {
				break;
			} else {
				// could be a reference / var
			lex_else:
				if (!lex_handle_ref(ctx, ch)) return 0;
				goto st_end;
			}
			break;
		}
		case ST_END: {
			if (is_whitespace(ch)) break;
		st_end:
			if (ch == '+') {
				build_add_node(ctx);
				ctx->status = ST_NONE;
				break;
			} else if (ch == '=') {
				ctx->status = ST_EQUAL_SYMBOL;
				break;
			}

			if (ch == ')') {
				// use the parent effectively
				if (is_part_of_type(ast, ctx->temp_node, CALL)) {
					ctx->temp_node->state = NS_END;

					uint32_t og_id = ast->len - 1;
					uint32_t temp_id = new_node(ast);
					ctx->temp_node = &ast->array[temp_id];
					ctx->temp_node->parent_id = ast->array[ast->array[og_id].parent_id].parent_id;
					ast->array[og_id].next_id = temp_id;

					ctx->status = ST_NONE;
				} else {
					return user_err("unexpected ')'");
				}
			} else if (ch == '{') {
				if (is_part_of_type(ast, ctx->temp_node, IF)) {
					ctx->temp_node->state = NS_END;

					uint32_t if_id = get_higher_id(ast, ctx->temp_node);
					uint32_t temp_id = new_node(ast);
					ctx->temp_node->next_id = temp_id;
					ctx->temp_node = &ast->array[temp_id];
					ctx->temp_node->parent_id = if_id;

					ctx->status = ST_NONE;
				} else {
					return user_err("unexpected '{'");
				}
			} else if (ch == '}') {
				if (is_part_of_type(ast, ctx->temp_node, IF)) {
					uint32_t if_id = get_higher_id(ast, ctx->temp_node);
					uint32_t temp_id = new_node(ast);
					ctx->temp_node->next_id = temp_id;
					ast->array[if_id].next_id = temp_id;
					ctx->temp_node = &ast->array[temp_id];

					ctx->status = ST_NONE;
				} else {
					return user_err("unexpected '{'");
				}

				// a new statement / ending
			} else if (is_lex(ch) || ch == '\0') {
				if (is_part_of_type(ast, ctx->temp_node, DECLARATION)) {
					ctx->temp_node->state = NS_END;

					uint32_t og_id = ast->len - 1;
					uint32_t temp_id = new_node(ast);
					ctx->temp_node = &ast->array[temp_id];
					ctx->temp_node->parent_id = ast->array[ast->array[og_id].parent_id].parent_id;
					ast->array[og_id].next_id = temp_id;

					ctx->status = ST_NONE;
					if (is_lex(ch)) return process(ctx, ch);
					break;
				}
				return user_err("unexpected character");
			} else {
				return user_err("end: unexpected character");
			}
			break;
		}
	}

#ifdef DEBUG
	printf("%c", ch);
	fflush(stdout);
#endif
	return 1;
}

uint8_t load_file(ctx_t *ctx, FILE *file) {
	char ch;
	while ((ch = fgetc(file)) != EOF) {
		if (!process(ctx, ch)) return 0;
	}
	if (!process(ctx, '\0')) return 0;
	if (ctx->status != ST_NONE) {
		return user_err("unfinished statement");
	}
	return 1;
}
// where would it even add code anyway?
// maybe onto the ast..
// would it need a second pointer for the entry point?
// hmm but it also needs to know the next instruction
