#include "runtime.h"
#include <string.h>

void run_func(func_t *func) {
	if (strcmp(func->name, "print") == 0) {
		string_t *content = func->params[0].value.ptr;
		printf("%s\n", content->array);
	}
}

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

uint8_t handle_call(node_t *temp_node) {
	func_t *func = temp_node->ptr;

	func->params[0].value = *(value_t*)temp_node->next->ptr;

	run_func(func);

	return 1;
}

uint8_t run_ast(ast_t *ast) {
	node_t *temp_node = &ast->array[0];
	while (temp_node->type != END) {
		switch (temp_node->type) {
			case BLOCK: {
				printf("BLOCK\n");
				break;
			}
			case CALL: {
				handle_call(temp_node);
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

uint8_t print_ast(ast_t *ast) {
	printf("-- AST STATS --\n");
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
				func_t *func = temp_node->ptr;
				printf("%s\n", func->name);
				break;
			}
			case PARENTHESIS: {
				printf("PARENTHESIS\n");
			}
			case LITERAL: {
				printf("LITERAL\n");
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


