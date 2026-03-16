#include "runtime.h"
#include "string.h"
#include "user_error.h"

#include <string.h>

// decide temp values for evaluation in stack
// so no need for inefficient free and malloc

// if something requires an exp such as call or declaration
// check if the child is an exp and not literal
// if the child is a number you must turn it into a string

uint8_t activate_node(interpreter_t *preter, uint32_t id) {
	ast_t ast = preter->ast;
	values_t values = preter->values;
	strings_t *strings = &preter->strings;
	rt_ints_t *rt_ints = &preter->rt_ints;

	node_t temp_node = ast.array[id];
	node_t parent = ast.array[temp_node.parent_id];
	value_t *parent_value = &values.array[parent.ptr];

	value_t value;
	if (temp_node.type == REFERENCE) {
		var_t var = preter->vars.array[temp_node.ptr];
		value = values.array[var.ptr];
	} else {
		value = values.array[temp_node.ptr];
	}

	// adding a string is ok
	// Not adding is string is Not ok
	// adding a number is ok
	// not adding a number is ok
	// == string && != add then stop

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
				break;
				rt_ints->i64s[parent_value->ptr] = rt_ints->i64s[value.ptr];
			}
			default: {
			}
		}
		parent_value->type = value.type;
		return 1;
	}

	value_t val_first;
	if (is_compare(parent.type)) {
		if (ast.array[parent.next_id].type == REFERENCE) {
			val_first = values.array[preter->vars.array[ast.array[parent.next_id].ptr].ptr];
		} else {
			val_first = values.array[ast.array[parent.next_id].ptr];
		}
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

uint8_t eval_exp(interpreter_t *preter, uint32_t id, uint32_t *end_id) {
	ast_t ast = preter->ast;
	node_t temp_node = ast.array[id];
	queue_t queue;
	queue.len = 0;

	while (temp_node.state != NS_END) {
		id = temp_node.next_id;
		temp_node = ast.array[id];

		if (should_eval(temp_node.type)) {
			add2queue(&queue, id);
		} else if (temp_node.type == CALL_PARAM) {
		} else {
			if (!activate_node(preter, id)) return 0;
			if (temp_node.state == NS_BACK || temp_node.state == NS_END) {
				if (queue.len > 0) {
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
				if (ast.array[temp_node->parent_id].type == FOR_LOOP) {
					node_t top = ast.array[temp_node->parent_id];
					uint32_t end_id;
					eval_exp(preter, temp_node->next_id, &end_id);
					value_t val = values.array[ast.array[temp_node->next_id].ptr];

					if (val.type == BOOL && val.ptr != 0) {
						id = ast.array[end_id].next_id;
						break;
					}
					id = temp_node->ptr;
				} else {
					id = temp_node->next_id;
				}
				break;
			}
			case CALL: {
				func_t *func = &preter->funcs.array[temp_node->ptr];
				uint32_t end_id;

				// first param
				if (!eval_exp(preter, id, &end_id)) return 0;
				func->params[0].ptr = ast.array[temp_node->next_id].ptr;

				if (strcmp(func->name, "print") == 0) {
					value_t value = preter->values.array[func->params[0].ptr];
					print_value(preter, value);
					printf("\n");
				}

				id = ast.array[end_id].next_id;
				break;
			}
			case DECLARATION: {
				var_t *var = &vars.array[temp_node->ptr];
				uint32_t end_id;

				eval_exp(preter, temp_node->next_id, &end_id);
				var->ptr = ast.array[temp_node->next_id].ptr;

				id = ast.array[end_id].next_id;
				break;
			}
			case IF: {
				uint32_t end_id;
				eval_exp(preter, temp_node->next_id, &end_id);
				value_t val = values.array[ast.array[temp_node->next_id].ptr];

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
				id = temp_node->ptr;
				break;
			}
			case END_LOOP: {
				uint32_t loop_id = temp_node->parent_id;
				node_t top = ast.array[loop_id];
				if (top.type == FOR_LOOP) {
					uint32_t end_id;
					eval_exp(preter, ast.array[top.next_id].next_id, &end_id);
					value_t val = values.array[ast.array[ast.array[top.next_id].next_id].ptr];

					if (val.type == BOOL && val.ptr != 0) {
						id = ast.array[end_id].next_id;
					} else {
						id = ast.array[top.next_id].ptr;
					}
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
	}
	printf("\nPTR: %d: ", temp_node.ptr);
	if (temp_node.type == VALUE || should_eval(temp_node.type)) {
		value_t value = preter->values.array[temp_node.ptr];
		print_value(preter, value);
		printf("\n");
		if (value.type != UNDEFINED) {
			printf("[%d]\n", value.ptr);
		}
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
