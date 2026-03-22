#include "runtime.h"
#include "string.h"
#include "user_error.h"

#include <string.h>

// decide temp values for evaluation in stack
// so no need for inefficient free and malloc

// if something requires an exp such as call or declaration
// check if the child is an exp and not literal
// if the child is a number you must turn it into a string

void print_value(interpreter_t *preter, value_t value) {
	switch (value.type) {
		case STRING: {
			string_t content = preter->strings.array[value.ptr];
			content.array[content.len] = '\0';
			printf("%s", content.array);
			break;
		}
		case I32: {
			int32_t i32 = preter->rt_ints.i32s[value.ptr];
			printf("%d", i32);
			break;
		}
		case I64: {
			int64_t i64 = preter->rt_ints.i64s[value.ptr];
			printf("%lld", i64);
			break;
		}
		case BOOL: {
			if (value.ptr) {
				printf("true");
			} else {
				printf("false");
			}
			break;
		}
		default: {
			break;
		}
	}
}

void call(interpreter_t *preter, node_t *node, uint16_t params[MAX_PARAM_LEN]) {
	uint16_t func_id = preter->ast.array[node->ptr].ptr;

	// print
	if (func_id == 0) {
		value_t value = preter->values.array[params[0]];
		print_value(preter, value);
		printf("\n");
	} else if (func_id == 1) {
		value_t value = preter->values.array[params[0]];
		print_value(preter, value);

		uint16_t out_val = new_value(&preter->values);
		preter->ast.array[node->ptr].next_id = out_val;
		uint16_t str_id = new_string(&preter->strings);
		preter->values.array[out_val].ptr = str_id;
		preter->values.array[out_val].type = STRING;

		char ch;
		while ((ch = fgetc(stdin)) != '\n') {
			add_char2string(&preter->strings.array[str_id], ch);
		}
	}
}

uint16_t node_value(interpreter_t *preter, uint32_t id) {
	node_t node = preter->ast.array[id];
	switch (node.type) {
		case REFERENCE: {
			return preter->vars.array[preter->ast.array[node.ptr].ptr].ptr;
		}
		case VALUE: {
			return node.ptr;
		}
		case CALL: {
			return preter->ast.array[node.ptr].next_id;
		}
		default: {
			return node.ptr;
		}
	}
}

void copy_value(interpreter_t *preter, uint16_t out_id, uint16_t in_id) {
	ast_t ast = preter->ast;
	values_t values = preter->values;
	strings_t *strings = &preter->strings;
	rt_ints_t *rt_ints = &preter->rt_ints;

	value_t *out = &values.array[out_id];
	value_t *in = &values.array[in_id];

	// delete original value if datatypes are different
	if (out->type != in->type) {
		switch (out->type) {
			case UNDEFINED:
				break;
			case STRING:
				delete_string(strings, out->ptr);
				break;
			case I32:
				delete_i32(rt_ints, out->ptr);
				break;
			case I64:
				delete_i64(rt_ints, out->ptr);
				break;
			case FLOAT:
				// WIP
				break;
			case BOOL:
				break;
		}

		out->type = in->type;
		switch (in->type) {
			case UNDEFINED:
				break;
			case STRING: {
				uint32_t str_id = new_string(strings);
				out->ptr = str_id;
				break;
			}
			case I32: {
				out->ptr = new_i32(rt_ints);
				break;
			}
			case I64: {
				out->ptr = new_i64(rt_ints);
				break;
			}
			case FLOAT:
				break;
			case BOOL:
				out->ptr = in->ptr;
				break;
		}
	}
	switch (out->type) {
		case UNDEFINED:
			break;
		case STRING:
			copy_string_w_id(strings, out->ptr, in->ptr);
			break;
		case I32:
			rt_ints->i32s[out->ptr] = rt_ints->i32s[in->ptr];
			break;
		case I64:
			rt_ints->i64s[out->ptr] = rt_ints->i64s[in->ptr];
			break;
		case FLOAT:
			break;
		case BOOL:
			out->ptr = in->ptr;
			break;
	}
}

uint8_t activate_node(interpreter_t *preter, uint32_t id) {
	ast_t ast = preter->ast;
	values_t values = preter->values;
	strings_t *strings = &preter->strings;
	rt_ints_t *rt_ints = &preter->rt_ints;

	node_t temp_node = ast.array[id];
	node_t parent = ast.array[temp_node.parent_id];
	value_t *parent_value = &values.array[parent.ptr];

	//if (parent.type == CALL) return 1;
	// we need to activate a function of necessary
	// function -> queue
	// function should only be called, maybe evaled if parent is a operator
	// parameters -> end needs to eval Twice. END_CALL state??
	if (temp_node.type == CALL) {
		// call and store value in block -> ptr
		uint16_t params[MAX_PARAM_LEN];
		params[0] = node_value(preter, temp_node.next_id);
		call(preter, &ast.array[id], params);
	}

	// adding a string is ok
	// Not adding is string is Not ok
	// adding a number is ok
	// not adding a number is ok
	// == string && != add then stop

	// should not modify parent pointer if parent were CALL or DECLARATION
	if (!should_eval(parent.type)) return 1;
	value_t value = values.array[node_value(preter, id)];

	if (parent.next_id == id) {
		if (is_compare(parent.type)) return 1;

		switch (value.type) {
			case STRING: {
				if (parent_value->type == UNDEFINED) {
					parent_value->ptr = new_string(strings);
				}
				copy_string_w_id(strings, parent_value->ptr, value.ptr);
				break;
			}
			case I32: {
				if (parent_value->type == UNDEFINED) {
					parent_value->ptr = new_i32(rt_ints);
				}
				rt_ints->i32s[parent_value->ptr] = rt_ints->i32s[value.ptr];
				break;
			}
			case I64: {
				if (parent_value->type == UNDEFINED) {
					parent_value->ptr = new_i64(rt_ints);
				}
				rt_ints->i64s[parent_value->ptr] = rt_ints->i64s[value.ptr];
				break;
			}
			default: {
			}
		}
		parent_value->type = value.type;
		return 1;
	}

	value_t val_first;
	if (is_compare(parent.type)) {
		val_first = values.array[node_value(preter, parent.next_id)];
	}
	switch (parent.type) {
		case ADD: {
			if (value.type == STRING && parent_value->type == STRING) {
				add_strings_w_id(strings, parent_value->ptr, value.ptr);
			} else if (value.type == I32 && parent_value->type == STRING) {
			} else if (value.type == I64 && parent_value->type == STRING) {
			}

			// number things
			if (value.type == I32 && parent_value->type == I32) {
				int32_t i32_1 = rt_ints->i32s[parent_value->ptr];
				int32_t i32_2 = rt_ints->i32s[value.ptr];

				if (i32_1 > INT32_MAX - i32_2) {
					// turn into i64
					delete_i32(rt_ints, parent_value->ptr);
					parent_value->ptr = new_i64(rt_ints);
					rt_ints->i64s[parent_value->ptr] += i32_2;
				} else {
					rt_ints->i32s[parent_value->ptr] = i32_1 + i32_2;
				}
			} else if (value.type == I64 && parent_value->type == I64) {
				int32_t i64_1 = rt_ints->i64s[parent_value->ptr];
				int32_t i64_2 = rt_ints->i64s[value.ptr];
				if (i64_1 > INT64_MAX - i64_2) {
					return user_err("integer is too large");
				}
				rt_ints->i64s[parent_value->ptr] += rt_ints->i64s[value.ptr];
			} else if (value.type == I32 && parent_value->type == I64) {
				rt_ints->i64s[parent_value->ptr] += rt_ints->i32s[value.ptr];
			} else if (value.type == I64 && parent_value->type == I32) {
				delete_i32(rt_ints, parent_value->ptr);
				parent_value->ptr = new_i64(rt_ints);
				int32_t i64_1 = rt_ints->i64s[parent_value->ptr];
				int32_t i64_2 = rt_ints->i64s[value.ptr];
				if (i64_1 > INT64_MAX - i64_2) {
					return user_err("integer is too large");
				}
				rt_ints->i64s[parent_value->ptr] += rt_ints->i64s[value.ptr];
			}
			break;
		}
		case EQUAL: {
			parent_value->ptr = numbers_equal(rt_ints, val_first.type, val_first.ptr, value.type, value.ptr);
			break;
		}
		case LESS: {
			parent_value->ptr = numbers_less(rt_ints, val_first.type, val_first.ptr, value.type, value.ptr);
			break;
		}
		case MORE: {
			parent_value->ptr = numbers_less(rt_ints, val_first.type, val_first.ptr, value.type, value.ptr);
			break;
		}
		default: {
		}
	}
	return 1;
}

typedef struct {
	uint8_t len;
	uint32_t array[MAX_QUEUE_LEN];
} queue_t;

void add2queue(queue_t *queue, uint32_t id) {
	queue->array[queue->len] = id;
	queue->len++;
	// take care of max queue issues at lexer-time
}

uint8_t eval_exp(interpreter_t *preter, uint32_t target_id, uint32_t *end_id) {
	ast_t ast = preter->ast;
	uint32_t id = target_id;
	node_t temp_node = ast.array[id];
	queue_t queue;
	queue.len = 0;

	while (temp_node.state != NS_END && temp_node.state != NS_END_CALL) {
		id = temp_node.next_id;
		temp_node = ast.array[id];

		if (should_eval(temp_node.type) || temp_node.type == CALL) {
			add2queue(&queue, id);
		} else {
			if (!activate_node(preter, id)) return 0;
			if (temp_node.state == NS_BACK || temp_node.state == NS_END) {
				if (queue.len > 0) {
					queue.len--;
					uint32_t node_id = queue.array[queue.len];
					if (!activate_node(preter, node_id)) return 0;
				}
			}
			if (temp_node.state == NS_END_CALL) {
				while (queue.len > 0) {
					queue.len--;
					uint32_t node_id = queue.array[queue.len];
					if (!activate_node(preter, node_id)) return 0;
				}
			}
		}

		/*if (temp_node.state == NS_BACK || temp_node.state == NS_END) {
			for (uint8_t i = 0; i < queue.len; i++){
				uint32_t node_id = queue.array[queue.len - i - 1];
				if (!activate_node(preter, node_id)) return 0;
			}
			queue.len = 0;
		}*/
	}
	*end_id = id;
	return 1;
}

// make it do math / evaluate
uint8_t run(interpreter_t *preter) {
	ast_t ast = preter->ast;
	vars_t vars = preter->vars;
	values_t values = preter->values;
	node_t *temp_node = &ast.array[0];
	uint32_t id = 0;

	printf("-- RUNTIME --\n");
	if (ast.len == 0) return 1;

	while (temp_node->type != END) {
		switch (temp_node->type) {
			case BLOCK: {
				if (ast.array[temp_node->parent_id].type == FOR_LOOP && ast.array[temp_node->parent_id].next_id == id) {
					node_t top = ast.array[temp_node->parent_id];
					uint32_t end_id;
					eval_exp(preter, ast.array[id].next_id, &end_id);
					value_t val = values.array[node_value(preter, ast.array[id].next_id)];

					if (val.type == BOOL && val.ptr != 0) {
						id = ast.array[end_id].next_id;
						break;
					}
					id = ast.array[top.ptr].ptr;
				} else {
					id = temp_node->next_id;
				}
				break;
			}
			case CALL: {
				func_t *func = &preter->funcs.array[ast.array[temp_node->ptr].ptr];
				uint32_t end_id;
				uint16_t params[MAX_PARAM_LEN];

				// first param
				// make functions have outputs by...
				// just use a get_value function to get reference ok of a node?
				if (!eval_exp(preter, id, &end_id)) return 0;
				params[0] = node_value(preter, ast.array[id].next_id);

				call(preter, &ast.array[id], params);

				id = ast.array[end_id].next_id;
				break;
			}
			case DECLARATION: {
				var_t *var = &vars.array[temp_node->ptr];
				uint32_t end_id;

				node_t next = ast.array[temp_node->next_id];
				if (next.type == REFERENCE) {
					copy_value(preter, ast.array[next.ptr].next_id, vars.array[ast.array[next.ptr].ptr].ptr);
					var->ptr = ast.array[next.ptr].next_id;
					end_id = temp_node->next_id;
				} else {
					eval_exp(preter, id, &end_id);
					var->ptr = node_value(preter, temp_node->next_id);
				}

				id = ast.array[end_id].next_id;
				break;
			}
			case IF: {
				uint32_t end_id;
				uint16_t val_id;
				eval_exp(preter, id, &end_id);
				value_t val = values.array[node_value(preter, ast.array[id].next_id)];

				// true
				if (val.type == BOOL && val.ptr != 0) {
					id = ast.array[end_id].next_id;
				} else {
					id = temp_node->ptr;
				}
				break;
			}
			case FOR_LOOP: {
				// do the init process
				id = ast.array[temp_node->ptr].next_id;
				break;
			}
			case END_LOOP: {
				uint32_t loop_id = temp_node->parent_id;
				node_t top = ast.array[loop_id];

				if (top.type == FOR_LOOP) {
					uint32_t end_id;
					uint16_t val_id;
					uint32_t condition_id = ast.array[top.next_id].next_id;
					eval_exp(preter, condition_id, &end_id);
					value_t val = values.array[node_value(preter, condition_id)];

					if (val.type == BOOL && val.ptr != 0) {
						id = ast.array[top.next_id].ptr;
						break;
					}
					id = temp_node->next_id;
				}
				break;
			}
			default: {
			}
		}
		temp_node = &ast.array[id];
	}
	printf("-- END PROGRAM --\n");
	return 1;
}

uint8_t print_node(interpreter_t *preter, uint32_t id) {
	ast_t ast = preter->ast;
	node_t temp_node = ast.array[id];
	printf("-- ");
	log_nodetype(temp_node.type);
	printf(" --\n");
	printf("IDs: this %d / parent %d / next %d\n", id, temp_node.parent_id, temp_node.next_id);
	printf("state:");
	switch (temp_node.state) {
		case NS_NONE:
			printf("none");
			break;
		case NS_BACK:
			printf("back");
			break;
		case NS_END:
			printf("end");
			break;
		case NS_END_CALL:
			printf("end call");
			break;
	}
	printf("\nPTR: %d: ", temp_node.ptr);
	if (temp_node.type == VALUE || should_eval(temp_node.type)) {
		value_t value = preter->values.array[temp_node.ptr];
		print_value(preter, value);
		printf("\n");
		if (value.type != UNDEFINED) {
			printf("[%d]\n", value.ptr);
		}
	} else if (temp_node.type == REFERENCE) {
		var_t var = preter->vars.array[preter->ast.array[temp_node.ptr].ptr];
		printf("(REFERENCE): %s", var.name);
	}
	printf("\n");
	return 1;
}

uint8_t print_chain(interpreter_t *preter) {
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	uint32_t id = 0;
	printf("STATS %d\n", ast.len);
	while (temp_node->type != END) {
		print_node(preter, id);
		id = temp_node->next_id;
		temp_node = &ast.array[id];
	}
	print_node(preter, id);
	return 1;
}

uint8_t print_unordered(interpreter_t *preter) {
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	for (uint32_t i = 0; i < ast.len; i++) {
		print_node(preter, i);
	}
	return 1;
}
