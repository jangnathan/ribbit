#include <emscripten.h>
#include <stdint.h>
#include <string.h>
#include "../interpreter/interpreter.h"
#include "../interpreter/runtime.h"
#include "../interpreter/user_error.h"

EMSCRIPTEN_KEEPALIVE
void process_source(char* str) {
	interpreter_t preter;
	interpreter_init(&preter);
	ctx_t ctx;
	ctx.preter = &preter;
	ctx_init(&ctx);

	if (!load_string(&ctx, str)) return;

	run(&preter);
	cleanup(&preter);
}

