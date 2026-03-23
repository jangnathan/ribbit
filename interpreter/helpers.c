#include "helpers.h"

uint8_t is_whitespace(char ch) {
	if (ch == '\n' || ch == ' ' || ch == '\t') {
		return 1;
	}
	return 0;
}
uint8_t is_lex(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) return 1;
	if (chn >= 64 && chn <= 90) return 1;
	if (chn >= 97 && chn <= 122) return 1;
	if (ch == '_') return 1;
	return 0;
}
uint8_t is_num(char ch) {
	uint8_t chn = (uint8_t)ch;
	if (chn >= 48 && chn <= 57) {
		return 1;
	}
	return 0;
}
uint8_t char2digit(char ch) {
	uint8_t chn = (uint8_t)ch;
	return chn - 48;
}
