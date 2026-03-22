#include "interpreter.h"
#include "constants.h"
#include "user_error.h"
#include "helpers.h"
#include "value.h"
#include <stdio.h>
#include <string.h>

uint32_t append_child(ast_t *ast, uint32_t temp_id) {
	uint32_t new_id = new_node(ast);
	node_t *new = &ast->array[new_id];
	new->parent_id = temp_id;
	ast->array[temp_id].next_id = new_id;
	return new_id;
}

void interpreter_init(interpreter_t *preter) {
	#ifdef DEBUG
	printf("init interpreter start\n");
	#endif

	preter->ast.len = 0;
	preter->ast.size = 64;
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
	ctx->temp_id = new_node(ast);
	ast->array[0].type = BLOCK;
	ctx->temp_id = append_child(ast, ctx->temp_id);
	ctx->i = 0;
	ctx->lex[0] = '\0';
	ctx->status = ST_NONE;

	#ifdef DEBUG
	printf("init ctx end\n");
	#endif
}

uint32_t find_ancestor_of_type(ast_t *ast, uint32_t id, enum NODE_TYPE type) {
	node_t node = ast->array[id];

	while (node.parent_id != 0) {
		id = node.parent_id;
		node = ast->array[id];
		if (node.type == type) return id;
	}
	return 0;
}

void build_top_node(ctx_t *ctx, enum NODE_TYPE type) {
	interpreter_t *preter = ctx->preter;
	ast_t *ast = &preter->ast;
	values_t *values = &preter->values;

	node_t *og_node = &ast->array[ctx->temp_id];
	uint32_t root_id = og_node->parent_id;
	node_t *root_node = &ast->array[root_id];
	while (should_eval(root_node->type)) {
		root_id = root_node->parent_id;
		root_node = &ast->array[root_id];
	}

	uint32_t oper_id = new_node(ast);
	node_t *oper = &ast->array[oper_id];
	oper->type = type;
	oper->ptr = new_value(values);
	values->array[oper->ptr].type = BOOL;

	root_node = &ast->array[root_id];

	oper->next_id = root_node->next_id;
	root_node->next_id = oper_id;
	oper->parent_id = root_id;

	uint32_t new_id = new_node(ast);
	node_t *new = &ast->array[new_id];
	new->parent_id = oper_id;
	new->state = NS_BACK;

	og_node = &ast->array[ctx->temp_id];
	og_node->next_id = new_id;
	ctx->temp_id = new_id;

	ctx->status = ST_NONE;
}

// follow PEMDAS
// use to insert itself in node, using data of last node

// md of PEMDAS
void build_md_node(ctx_t *ctx, enum NODE_TYPE type) {
	interpreter_t *preter = ctx->preter;
	ast_t *ast = &preter->ast;
	values_t *values = &preter->values;

	uint32_t temp_id = ctx->temp_id;
	node_t temp = ast->array[temp_id];

	uint32_t copy_id = new_node(ast);
	ast->array[copy_id].parent_id = temp_id;
	ast->array[copy_id].ptr = temp.ptr;
	ast->array[copy_id].type = temp.type;

	ast->array[temp_id].type = type;
	ast->array[temp_id].ptr = new_value(values);
	ast->array[temp_id].next_id = copy_id;

	uint32_t new_id = new_node(ast);
	ast->array[new_id].parent_id = temp_id;
	ast->array[copy_id].next_id = new_id;

	ctx->temp_id = new_id;
	ast->array[new_id].state = NS_BACK;

	ctx->status = ST_NONE;
}

void build_as_node(ctx_t *ctx, enum NODE_TYPE type) {
	interpreter_t *preter = ctx->preter;
	ast_t *ast = &preter->ast;
	values_t *values = &preter->values;

	uint32_t temp_id = ctx->temp_id;
	node_t temp = ast->array[temp_id];
	
	if (ast->array[temp.parent_id].type >= DIV) {
		build_md_node(ctx, type);
		return;
	}

	// highest node of PEMD of PEMDAS
	while (ast->array[temp.parent_id].type <= DIV && should_eval(ast->array[temp.parent_id].type)) {
		temp_id = temp.parent_id;
		temp = ast->array[temp_id];
	}

	uint32_t op_id = new_node(ast);
	ast->array[op_id].type = type;
	ast->array[op_id].ptr = new_value(values);
	ast->array[op_id].next_id = temp_id;
	ast->array[op_id].parent_id = temp.parent_id;
	ast->array[temp.parent_id].next_id = op_id;
	ast->array[op_id].state = 0;
	ast->array[temp_id].parent_id = op_id;

	uint32_t new_id = new_node(ast);
	ast->array[new_id].parent_id = op_id;
	ast->array[new_id].state = NS_BACK;
	ast->array[ctx->temp_id].next_id = new_id;

	ctx->temp_id = new_id;

	ctx->status = ST_NONE;
}

uint8_t build_ref_node(ctx_t *ctx) {
	interpreter_t *preter = ctx->preter;
	int32_t var_id = get_var(&preter->vars, ctx->lex);
	if (var_id == -1) {
		printf("'%s'", ctx->lex);
		return user_err("variable doesnt exist");
	}

	preter->ast.array[ctx->temp_id].type = REFERENCE;
	uint32_t new_id = new_node(&preter->ast);
	preter->ast.array[ctx->temp_id].ptr = new_id;
	// next_id is to store temp , ptr is for reference
	preter->ast.array[new_id].ptr = var_id;
	preter->ast.array[new_id].type = BLOCK;
	preter->ast.array[new_id].next_id = new_value(&preter->values);
	return 1;
}

void end_statement(ctx_t *ctx) {
	ast_t *ast = &ctx->preter->ast;
	uint32_t top_id = ctx->temp_id;
	while (!is_statement(ast->array[top_id].type) && top_id != 0) {
		top_id = ast->array[top_id].parent_id;
	}
	// no statements found
	if (top_id == 0) return;

	ast->array[ctx->temp_id].state = NS_END;

	uint32_t new_id = new_node(ast);
	node_t *new_node = &ast->array[new_id];
	ast->array[ctx->temp_id].next_id = new_id;

	ctx->temp_id = new_id;
	new_node->parent_id = ast->array[top_id].parent_id;
}

uint8_t end_curly_braces(ctx_t *ctx) {
	interpreter_t *preter = ctx->preter;
	funcs_t *funcs = &preter->funcs;
	vars_t *vars = &preter->vars;
	values_t *values = &preter->values;
	strings_t *strings = &preter->strings;
	ast_t *ast = &preter->ast;

	uint32_t top_id = ctx->temp_id;
	while (!has_curly_braces(ast->array[top_id].type) && top_id != 0) {
		top_id = ast->array[top_id].parent_id;
		log_nodetype(ast->array[top_id].type);
		printf("(%d)", top_id);
	}
	if (top_id == 0) return user_err("unexpected '}'");

	end_statement(ctx);
	ctx->status = ST_NONE;

	switch (ast->array[top_id].type) {
		case IF:
			ast->array[ctx->temp_id].parent_id = ast->array[top_id].parent_id;
			ast->array[top_id].ptr = ctx->temp_id;
			break;
		case FOR_LOOP: {
			ast->array[ctx->temp_id].type = END_LOOP;
			ast->array[ctx->temp_id].parent_id = top_id;
			uint32_t new_id = new_node(ast);
			ast->array[new_id].parent_id = ast->array[top_id].parent_id;
			ast->array[ctx->temp_id].next_id = new_id;
			ast->array[ast->array[top_id].ptr].ptr = new_id;
			ctx->temp_id = new_id;
			break;
		}
		default: return user_err("unexpected '}'");
	}
	return 1;
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
				if (!build_ref_node(ctx)) return 0;
				build_top_node(ctx, EQUAL);
			} else {
				if (strcmp(ctx->lex, "true") == 0) {
					return user_err("cannot set variable name to true");
				}
				if (strcmp(ctx->lex, "false") == 0) {
					return user_err("cannot set variable name to false");
				}
				ast->array[ctx->temp_id].type = DECLARATION;
				int32_t var_id = get_var(vars, ctx->lex);
				if (var_id == -1) {
					var_id = new_var(vars, ctx->lex);
				}
				ast->array[ctx->temp_id].ptr = var_id;
				ctx->temp_id = append_child(ast, ctx->temp_id);
				ctx->status = ST_NONE;
				goto st_none;
			}
			break;
		}
		case ST_EQUAL_SYMBOL: {
			if (ch == '=') {
				build_top_node(ctx, EQUAL);
			} else {
				return user_err("unexpected = symbol");
			}
			break;
		}
		case ST_NONE: {
		st_none:
			ctx->i = 0;
			if (is_lex(ch) && !is_num(ch)) {
				ctx->status = ST_LEX;
				goto lex;
			} else if (ch == '"') {
				ctx->status = ST_STRING;

				ast->array[ctx->temp_id].type = VALUE;
				uint16_t id = new_value(values);
				value_t *value = &values->array[id];
				value->type = STRING;
				ast->array[ctx->temp_id].ptr = id;

				id = new_string(strings);
				value->ptr = id;
			} else if (is_num(ch) || ch == '-') {
				ctx->status = ST_NUMBER;

				ast->array[ctx->temp_id].type = VALUE;
				uint16_t id = new_value(values);
				value_t *value = &values->array[id];
				value->type = I32;
				ast->array[ctx->temp_id].ptr = id;

				if (ch == '-') {
					ctx->lex[0] = '-';
					ctx->i = 1;
				}

				// create nums later
				goto st_int;
			} else if (ch == '}') {
				if (!end_curly_braces(ctx)) return 0;
			} else if (is_whitespace(ch) || ch == '\0') {
			} else {
				printf("'%c'", ch);
				return user_err("unexpected char");
			}
			break;
		}
		case ST_NUMBER: {
			if (is_num(ch)) {
			st_int:
				ctx->lex[ctx->i] = ch;
				ctx->i++;
			} else {
				value_t *value = &values->array[ast->array[ctx->temp_id].ptr];
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

				ctx->status = ST_END;
				goto st_end;
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

				if (strcmp(ctx->lex, "if") == 0) {
					ast->array[ctx->temp_id].type = IF;

					// the condition
					// if false, then jump to id in ptr
					uint32_t new_id = new_node(ast);
					node_t *new = &ast->array[new_id];
					new->parent_id = ctx->temp_id;
					ast->array[ctx->temp_id].next_id = new_id;
					ctx->temp_id = new_id;
					ctx->status = ST_NONE;

					goto st_none;
				}
				if (strcmp(ctx->lex, "for") == 0) {
					ast->array[ctx->temp_id].type = FOR_LOOP;

					uint32_t block_id = new_node(ast);
					ast->array[block_id].type = BLOCK;
					ast->array[block_id].parent_id = ctx->temp_id;
					ast->array[ctx->temp_id].ptr = block_id;
					uint32_t new_id = new_node(ast);
					ast->array[block_id].next_id = new_id;
					ast->array[new_id].parent_id = block_id;
					ctx->temp_id = new_id;
					ctx->status = ST_NONE;

					goto st_none;
				}

				if (ch == '(') {
					ctx->status = ST_NONE;
					ast->array[ctx->temp_id].type = CALL;

					// if its a function
					int32_t func_id = get_func(funcs, ctx->lex);
					if (func_id == -1) return user_err("function doesnt exist");
					uint32_t new_id = new_node(ast);
					ast->array[ctx->temp_id].ptr = new_id;
					ast->array[new_id].ptr = func_id;
					ast->array[new_id].parent_id = ctx->temp_id;
					ast->array[new_id].type = BLOCK;

					func_t func = funcs->array[func_id];
					if (func.n_param > 0) {
						ctx->temp_id = append_child(ast, ctx->temp_id);
					}
				} else {
					// could be just a variable
					node_t *temp_node = &ast->array[ctx->temp_id];

					if (strcmp(ctx->lex, "true") == 0) {
						temp_node->type = VALUE;
						uint16_t value_id = new_value(&ctx->preter->values);
						temp_node->ptr = value_id;
						ctx->preter->values.array[value_id].type = BOOL;
						ctx->preter->values.array[value_id].ptr = 1;

						ctx->status = ST_END;
						goto st_end;
					} else if (strcmp(ctx->lex, "false") == 0) {
						temp_node->type = VALUE;
						uint16_t value_id = new_value(&ctx->preter->values);
						temp_node->ptr = value_id;
						ctx->preter->values.array[value_id].type = BOOL;
						ctx->preter->values.array[value_id].ptr = 0;

						ctx->status = ST_END;
						goto st_end;
					}

					ctx->status = ST_LEX_END;
					goto st_lex_end;
				}
			}
			break;
		}
		case ST_LEX_END: {
		st_lex_end:
			if (is_whitespace(ch)) break;

			if (ch == '=') {
				ctx->status = ST_EQUAL_SYMBOL_LEX;
			} else {
				if (!build_ref_node(ctx)) return 0;
				ctx->status = ST_END;
				goto st_end;
			}
			break;
		}
		case ST_STRING: {
			value_t value = values->array[ast->array[ctx->temp_id].ptr];
			string_t *str = &strings->array[value.ptr];
			if (ch == '"') {
				ctx->status = ST_END;
				str->array[str->len] = '\0';
			} else {
				add_char2string(str, ch);
			}
			break;
		}
		case ST_END: {
		st_end:
			if (is_whitespace(ch)) break;
			if (ch == '+') {
				build_as_node(ctx, ADD);
				break;
			}
			if (ch == '-') {
				build_as_node(ctx, SUB);
				break;
			}
			if (ch == '*') {
				build_md_node(ctx, MUL);
				break;
			}
			if (ch == '/') {
				build_md_node(ctx, DIV);
				break;
			}
			if (ch == '<') {
				build_top_node(ctx, LESS);
				break;
			}
			if (ch == '>') {
				build_top_node(ctx, MORE);
				break;
			}
			if (ch == '=') {
				ctx->status = ST_EQUAL_SYMBOL;
				break;
			}

			if (ch == ')') {
				// use the parent effectively
				uint32_t top_id = ctx->temp_id;
				while (!has_end_parenthesis(ast->array[top_id].type) && top_id != 0) {
					top_id = ast->array[top_id].parent_id;
				}
				if (top_id == 0) return user_err(" 1unexpected ')'");

				switch (ast->array[top_id].type) {
					case CALL: {
						ast->array[ctx->temp_id].state = NS_END_CALL;
						break;
					}
					default:
						return user_err(" 2unexpected ')'");
				}
			} else if (ch == ',') {
				uint32_t loop_id = find_ancestor_of_type(ast, ctx->temp_id, FOR_LOOP);
				if (loop_id != 0) {
					if (ast->array[loop_id].next_id == 0) {
						end_statement(ctx);
						// temp id is a new node
						// block
						ast->array[ctx->temp_id].type = BLOCK;
						ast->array[ctx->temp_id].parent_id = loop_id;
						ast->array[loop_id].next_id = ctx->temp_id;

						uint32_t new_id = new_node(ast);
						ast->array[ctx->temp_id].next_id = new_id;
						ast->array[new_id].parent_id = ctx->temp_id;

						// move onto conditionings
						ast->array[new_id].ptr = new_value(values);
						ctx->temp_id = new_id;
					} else {
						ast->array[ctx->temp_id].state = NS_END;
						ast->array[ast->array[loop_id].ptr].ptr = ctx->temp_id;

						// incremental
						uint32_t new_id = new_node(ast);
						ast->array[ast->array[loop_id].next_id].ptr = new_id;
						ast->array[new_id].parent_id = ast->array[loop_id].next_id;

						ctx->temp_id = new_id;
						ctx->status = ST_NONE;
					}
				} else {
					return user_err("unexpected ','");
				}
			} else if (ch == '{') {
				uint32_t top_id = ctx->temp_id;
				while (!has_curly_braces(ast->array[top_id].type) && top_id != 0) {
					top_id = ast->array[top_id].parent_id;
				}
				if (top_id == 0) return user_err("2unexpected '{'");

				switch (ast->array[top_id].type) {
					case IF: {
						ast->array[ctx->temp_id].state = NS_END;

						uint32_t new_id = new_node(ast);
						node_t *new_node = &ast->array[new_id];

						ast->array[ctx->temp_id].next_id = new_id;
						ctx->temp_id = new_id;
						new_node->parent_id = top_id;

						ctx->status = ST_NONE;
						break;
					}
					case FOR_LOOP: {
						ast->array[ctx->temp_id].state = NS_END;

						uint32_t new_id = new_node(ast);
						node_t *new_node = &ast->array[new_id];

						// remember this was a temporary use of ptr ptr, used for the end of condition node
						ast->array[ast->array[ast->array[top_id].ptr].ptr].next_id = new_id;
						ast->array[ctx->temp_id].next_id = ast->array[top_id].next_id;
						ctx->temp_id = new_id;
						ast->array[new_id].parent_id = top_id;
						ctx->status = ST_NONE;
						break;
					}
					default:
						return user_err("3unexpected '{'");
				}
			} else if (ch == '}') {
				if (!end_curly_braces(ctx)) return 0;
				// a new statement / ending
			} else if (is_lex(ch) || ch == '\0') {
				end_statement(ctx);
				ctx->status = ST_NONE;
				goto st_none;
			} else {
				printf("'%c'\n", ch);
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
