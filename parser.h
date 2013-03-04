#ifndef _PARSER_H_
#define _PARSER_H_

#include "tokenadt.h"
#include "numdict.h"

/* 
 * 归约函数，对词法分析完成后产生的 token 串进行归约
 * @token_array token 串
 * @token_stack 归约栈
 * @dict 变量表
 */
void reduction(tokenadt *token_array, tokenadt *token_stack, numdict *dict);

#endif 