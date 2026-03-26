#pragma once

#define RED_CONSOLE "\x1b[31m"
#define WHITE_CONSOLE "\x1b[0m"

#define MAX_LEX_LEN 32
#define MAX_QUEUE_LEN 256 // this constant is to control size of expressions e.g. 1 + 1 + 1,
// it is kept small because code can be kept readable
#define MAX_PARAM_LEN 16

//#define DEBUG
