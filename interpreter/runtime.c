#include "runtime.h"

uint8_t run_ast(ast_t *ast) {
	while (temp_node->type != END) {
		switch (temp_node->type) {
			case BLOCK: {
				break;
			}
			case CALL: {
				break;
			}
			case STRING: {
				break;
			}
			case END: {
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
	while (temp_node->type != END) {
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
			case STRING: {
				printf("STRING\n");
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


