#ifndef _TOKEN_ADT_H_
#define _TOKEN_ADT_H_

/*
 *tokens code
 *-----------------------------------------------
 * + | - | * | / | ( | ) | i | c | ? | = | # | N
 *-----------------------------------------------
 * 1   2   3   4   5   6   7   8   9   10  11  12
 */

 enum {
    T_PLUS = 1, T_MINUS, T_MULTI, T_DIVI, T_BRACES_L, T_BRACES_R,
    T_VAR, T_CONST, T_PRINT, T_ASSING, T_BOUND, T_N
};


/*
 * token 二元组
 * @code 种别码
 * @value 值，以字符串形式存放
 */
typedef struct {
    int code;
    //char value[20];
    void *value;
} token;


/*
 * token 串，在词法分析时形成：
 * @size: 串容量大小，一旦声明后不可改变，值大于0
 * @use: 已经使用的串容量，等于该串的长度
 */
typedef struct {
    size_t size;
    size_t use;
    token *items;
} tokenadt;


/* 创建类似数组的 token 结构，其中每一个元素是一个 token 二元组
 * 
 * @size token 串最大容量
 *
 * return 指向结构的指针
 */
tokenadt *token_adt_new(size_t size);

/* 重置 token 串结构 */
void token_adt_reset(tokenadt *tokens);

/* 删除 token 串 */
void token_adt_delete(tokenadt *tokens);

/* 添加 token 进 token 串
 * 
 * @tokens 指向 token 串的指针
 * @item token 二元组
 *
 * return 0 添加失败
 * return 1 添加成功
 */
int token_adt_append(tokenadt *tokens, token *item);

#endif