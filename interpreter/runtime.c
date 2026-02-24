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

	if (parent.type != ADD && value.type == STRING) {
		return user_err("Cannot perform any other arithmetic with string than add");
	}

	if (parent.next_id == id) {
		switch (value.type) {
			case STRING: {
				if (parent_value->type == UNDEFINED) {
					parent_value->ptr = new_string(strings);
				}
				copy_string_w_id(strings, parent_value->ptr, value.ptr);
			}
			default: {
			}
		}
		parent_value->type = value.type;
		return 1;
	}

	switch (parent.type) {
		case ADD: {
			if (value.type == STRING && parent_value->type == STRING) {
				add_strings_w_id(strings, parent_value->ptr, value.ptr);
			}
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

uint8_t eval_exp(interpreter_t *preter, uint32_t node_id) {
	ast_t ast = preter->ast;
	node_t temp_node = ast.array[node_id];
	uint32_t id = node_id;
	queue_t queue;
	queue.len = 0;

	if (!should_eval(temp_node.type)) {
		if (temp_node.type == REFERENCE) {
			var_t var = preter->vars.array[temp_node.ptr];
			ast.array[node_id].ptr = var.ptr;
		}
		return 1;
	}

	while (temp_node.state != NS_END) {
		id = temp_node.next_id;
		temp_node = ast.array[id];

		if (should_eval(temp_node.type)) {
			add2queue(&queue, id);
		} else {
			if (!activate_node(preter, id)) return 0;
		}

		if (temp_node.state == NS_BACK || temp_node.state == NS_END) {
			for (uint8_t i = 0; i < queue.len; i++){
				uint32_t node_id = queue.array[queue.len - i - 1];
				if (!activate_node(preter, node_id)) return 0;
			}
			queue.len = 0;
		}
	}
	return 1;
}

void print_value(interpreter_t *preter, value_t value) {
	switch (value.type) {
		case STRING: {
			string_t content = preter->strings.array[value.ptr];
			content.array[content.len] = '\0';
			printf("%s\n", content.array);
			break;
		}
		case I32: {
			int32_t i32 = preter->rt_ints.i32s[value.ptr];
			printf("%d\n", i32);
			break;
		}
		case I64: {
			int64_t i64 = preter->rt_ints.i64s[value.ptr];
			printf("%lld\n", i64);
			break;
		}
		default: {
			break;
		}
	}
}

uint8_t handle_call(interpreter_t *preter, node_t *temp_node) {
	ast_t ast = preter->ast;
	func_t *func = &preter->funcs.array[temp_node->ptr];

	// first param
	if (!eval_exp(preter, temp_node->next_id)) return 0;
	func->params[0].ptr = ast.array[temp_node->next_id].ptr;

	if (strcmp(func->name, "print") == 0) {
		value_t value = preter->values.array[func->params[0].ptr];
		print_value(preter, value);
	}

	return 1;
}

uint8_t handle_declare(interpreter_t *preter, node_t *temp_node) {
	ast_t ast = preter->ast;
	vars_t vars = preter->vars;
	var_t *var = &vars.array[temp_node->ptr];

	eval_exp(preter, temp_node->next_id);

	var->ptr = ast.array[temp_node->next_id].ptr;
	return 1;
}

uint8_t run(interpreter_t *preter) {
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	printf("-- RUNTIME --\n");
	while (temp_node->type != END) {
		switch (temp_node->type) {
			case BLOCK: {
				printf("BLOCK\n");
				break;
			}
			case CALL: {
				handle_call(preter, temp_node);
				break;
			}
			case DECLARATION: {
				handle_declare(preter, temp_node);
				break;
			}
			default: {
			}
		}

		temp_node = &ast.array[temp_node->next_id];
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
	if (temp_node.type == VALUE) {
		printf("\nVALUE: ");
		value_t value = preter->values.array[temp_node.ptr];
		print_value(preter, value);
	}
	printf("\n");
	return 1;
}

uint8_t print(interpreter_t *preter) {
	printf("-- AST STATS --\n");
	ast_t ast = preter->ast;
	uint32_t id = 0;
	node_t *temp_node = &ast.array[id];
	printf("ast len: %d\n", ast.len);
	printf("-- TREE --\n");
	uint8_t idx = 0;
	while (temp_node->type != END) {
		for (uint8_t i = 0; i < idx; i++) {
			printf("  ");
		}
		print_node(preter, id);

		if (temp_node->state > 0) {
			idx--;
		}
		if (temp_node->next_id != 0 && temp_node->state == NS_END) {
			idx++;
		}

		id = temp_node->next_id;
		temp_node = &ast.array[id];
	}
	return 1;
}

uint8_t print_chain(interpreter_t *preter) {
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	uint32_t id = 0;
	while (temp_node->type != END) {
		print_node(preter, id);
		id = temp_node->next_id;
		temp_node = &ast.array[id];
	}
	return 1;
}
