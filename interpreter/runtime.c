#include "runtime.h"
#include <string.h>

// decide temp values for evaluation in stack
// so no need for inefficient free and malloc

// if something requires an exp such as call or declaration
// check if the child is an exp and not literal
// if the child is a number you must turn it into a string
typedef struct {
	node_t *array[MAX_QUEUE_LEN];
	uint8_t len;
} queue_t;

void add2queue(queue_t *queue, node_t *node) {
	queue->array[queue->len] = node;
	queue->len++;
	// take care of max queue issues at lexer-time
}

uint8_t should_eval(enum NODE_TYPE type) {
	if (type >= ADD) {
		return 1;
	}
	return 0;
}

/*
uint8_t eval_exp(node_t *node) {
	node_t *temp_node = node;
	queue_t queue;
	queue.len = 0;
	while (temp_node->back == 0) {
		if (should_eval(temp_node->type)) {
			add2queue(&queue, temp_node);
		} else {
			switch (temp_node->parent->type) {
				case ADD: {
				}
				default: {
				}
			}
		}

		temp_node = temp_node->next;
	}
	return 1;
}
*/

uint8_t handle_call(interpreter_t *preter, node_t *temp_node) {
	ast_t ast = preter->ast;
	func_t *func = &preter->funcs.array[temp_node->ptr];

	// first param
	func->params[0].ptr = ast.array[temp_node->next_id].ptr;

	if (strcmp(func->name, "print") == 0) {
		value_t value = preter->values.array[func->params[0].ptr];
		string_t content = preter->strings.array[value.ptr];
		printf("%s\n", content.array);
	}

	return 1;
}

uint8_t handle_declare(interpreter_t *preter, node_t *temp_node) {
	ast_t ast = preter->ast;
	temp_node->ptr = ast.array[temp_node->next_id].ptr;
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

void print_value(interpreter_t *preter, value_t *value) {
	switch (value->type) {
		case STRING: {
			string_t str = preter->strings.array[value->ptr];
			printf("STRING: %s\n", str.array);
			break;
		}
		default: {
		}
	}
	printf("VALUE\n");

}
uint8_t print(interpreter_t *preter) {
	printf("-- AST STATS --\n");
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	printf("ast len: %d\n", ast.len);
	printf("-- TREE --\n");
	uint8_t idx = 0;
	while (temp_node->type != END) {
		temp_node = &ast.array[temp_node->next_id];

		for (uint8_t i = 0; i < idx; i++) {
			printf("  ");
		}
		switch (temp_node->type) {
			case BLOCK: {
				printf("BLOCK\n");
				break;
			}
			case CALL: {
				func_t func = preter->funcs.array[temp_node->ptr];
				printf("%s\n", func.name);
				break;
			}
			case DECLARATION: {
				printf("DECLARATION\n");
				break;
			}
			case PARENTHESIS: {
				printf("PARENTHESIS\n");
				break;
			}
			case VALUE: {
				value_t value = preter->values.array[temp_node->ptr];
				print_value(preter, &value);
				break;
			}
			case END: {
				printf("END\n");
				break;
			}
			default: {
			}
		}

		if (temp_node->back == 1) {
			idx--;
		}
		if (temp_node->next_id != 0 && temp_node->back == 0) {
			idx++;
		}
	}
	return 1;
}

uint8_t print_chain(interpreter_t *preter) {
	ast_t ast = preter->ast;
	node_t *temp_node = &ast.array[0];
	while (temp_node->type != END) {
		log_nodetype(temp_node->type);
		printf("-->");
		if (temp_node->back) {
			printf("BACK |\n");
		}
		temp_node = &ast.array[temp_node->next_id];
	}
	return 1;
}
