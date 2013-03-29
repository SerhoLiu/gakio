#ifndef _LEX_H_
#define _LEX_H_

#include "tokenadt.h"
#include "vpool.h"

/*
 * 词法分析函数
 * @tokens 传入参数，用于返回生成的 token 串
 * @vpool 集中的对象管理池
 * @codes 表达式
 * return 1 错误
 * return 0 正确返回
 */
int akio_lex(tokenadt *tokens, valuepool *vpool, char *codes);

#endif 