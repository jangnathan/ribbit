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

uint8_t handle_call(interpreter_t *preter, node_t *temp_node) {
	func_t *func = &preter->funcs.array[temp_node->ptr];

	// first param
	func->params[0].value = preter->values.array[temp_node->next->ptr];

	if (strcmp(func->name, "print") == 0) {
		value_t value = func->params[0].value;
		string_t content = preter->strings.array[value.ptr];
		printf("%s\n", content.array);
	}

	return 1;
}

uint8_t run(interpreter_t *preter) {
	ast_t *ast = &preter->ast;
	node_t *temp_node = &ast->array[0];
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
			case END: {
				printf("END\n");
				break;
			}
			default: {
			}
		}

		temp_node = temp_node->next;
	}
	return 1;
}

void print_literal(interpreter_t *preter, value_t *value) {
	switch (value->type) {
		case STRING: {
			string_t str = preter->strings.array[value->ptr];
			printf("STRING: %s\n", str.array);
			break;
		}
		default: {
		}
	}
	printf("LITERAL\n");

}
uint8_t print(interpreter_t *preter) {
	printf("-- AST STATS --\n");
	ast_t *ast = &preter->ast;
	node_t *temp_node = &ast->array[0];
	printf("ast len: %d\n", ast->len);
	printf("-- TREE --\n");
	uint8_t idx = 0;
	while (temp_node->type != END) {
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
			case PARENTHESIS: {
				printf("PARENTHESIS\n");
			}
			case LITERAL: {
				value_t value = preter->values.array[temp_node->ptr];
				print_literal(preter, &value);
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
		if (temp_node->next != 0 && temp_node->back == 0) {
			idx++;
		}

		temp_node = temp_node->next;
	}
	return 1;
}

uint8_t print_chain(interpreter_t *preter) {
	ast_t *ast = &preter->ast;
	node_t *temp_node = &ast->array[0];
	while (temp_node->type != END) {
		log_nodetype(temp_node->type);
		printf("-->");
		if (temp_node->back) {
			printf("BACK |\n");
		}
		temp_node = temp_node->next;
	}
	return 1;
}
