#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "gakio.h"

#define TOKENNUM 11
#define GRE 1
#define EQU 0
#define LE -1
#define NONE -2


/* 算符优先关系表 Todo: 使用算符优先函数 */ 
static const int token_pri[TOKENNUM][TOKENNUM] = {
    /* +   -    *   /  (    )   i   c   ?     =      #  */
    {GRE, GRE, LE, LE, LE, GRE, LE, LE, NONE, NONE, GRE},          // +
    {GRE, GRE, LE, LE, LE, GRE, LE, LE, NONE, NONE, GRE},          // -
    {GRE, GRE, GRE, GRE, LE, GRE, LE, LE, NONE, NONE, GRE},        // *
    {GRE, GRE, GRE, GRE, LE, GRE, LE, LE, NONE, NONE, GRE},        // /
    {LE, LE, LE, LE, LE, EQU, LE, LE, NONE, NONE, NONE},           // (
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, NONE, GRE},  // )
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, LE, GRE},    // i
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, NONE, GRE},  // c
    {LE, LE, LE, LE, LE, NONE, LE, LE, NONE, NONE, GRE},           // ?
    {LE, LE, LE, LE, LE, NONE, LE, LE, NONE, NONE, GRE},           // =
    {LE, LE, LE, LE, LE, NONE, LE, LE, LE, LE, EQU}                // #
};


/* N <= i
 * @t 变量 i
 * @dict 变量表
 *
 * return VAR_NOT_DEFINE 变量 i 没有定义
 * return 0 成功
 */
static int var_reduction(token *t, numdict *dict)
{
    //printf("numdict get red\n");
    void *value;
    value = numdict_get(dict, t->value);
    if (value == NULL) {
        return VAR_NOT_DEFINE;
    }
    t->value = value;
    t->code = T_N;
    return 0;
}


/* 
 * N <= N +|-|*|/ N 
 * @l 左操作数
 * @r 右操作数
 * @op 操作符
 * @vpool 集中的对象管理池
 * 结果将修改 l或者op 作为返回值
 *
 * return LACK_OPER 缺少操作数
 * return DIVZERO   除 0 错误
 * return 1 - 符缺少左操作数，以负数处理
 * return 0 正常
 */
static int arith_reduction(token *l, token *r, token *op, valuepool *vpool)
{
    int isneg = 0;

    GakioNum left, right, *result = NULL;
    left = right = 0;

    /* 无右操作数错误处理 */
    if (r->code != 12) {
        return LACK_OPER;
    }

    if (l->code != T_N) {
        /* 负数处理：如果是 -,添 0 */
        if (op->code == T_MINUS) {
            left = 0;
            isneg = 1;
        } else {
            return LACK_OPER;
        } 
    } else {
        left = *(GakioNum *)(l->value);
    }
    
    right = *(GakioNum *)(r->value);

    result = (GakioNum *)malloc(sizeof(GakioNum));
    
    switch (op->code) {
        case T_PLUS: {
            *result = left + right;
            break;
        }
        case T_MINUS: {
            *result = left - right;
            break;
        }
        case T_MULTI: {
            *result = left * right;
            break;
        }
        case T_DIVI: {
            if (right == 0) {
                return DIVZERO;
            }
            *result = left / right;
            break;
        }
        default:  {
            free(result);
            return INVALID_OP;
        }
    }

    value_pool_append(vpool, MAKE_GAKIONUM(result));
    if (isneg) {
        op->code = T_N;
        op->value = result;  
    } else {
        l->code = T_N;
        l->value = result;
    }
    return isneg;
}


/*
 * N <= i = N 
 * @i 变量
 * @n N
 * @dict 变量表
 */
static void value_reduction(token *i, token *n, numdict *dict)
{
    numdict_put(dict, i->value, n->value);
    n->code = T_N;
}


static void code_to_op_string(int code, char *str)
{
    switch (code) {
        case T_PLUS:  {str[0] = '+'; break;}
        case T_MINUS: {str[0] = '-'; break;}
        case T_MULTI: {str[0] = '*'; break;}
        case T_DIVI:  {str[0] = '/'; break;}
        case T_BRACES_L: {str[0] = '('; break;}
        case T_BRACES_R: {str[0] = ')'; break;}
        default: {str[0] = '!';}
    }
    str[1] = '\0';
}


void reduction(tokenadt *token_array, tokenadt *token_stack, 
                numdict *dict, valuepool *vpool)
{ 
    token ta, ts;
    
    /* 将 # 入栈 */
    ts.code = T_BOUND;
    ts.value = NULL;
    token_adt_append(token_stack, &ts);
    
    size_t idx = 0;
    size_t s_top, j;
    while (idx < token_array->use) {
        ta.code = token_array->items[idx].code;
        
        s_top = token_stack->use - 1;

        /* 判断栈顶元素是否在符号表中 */         
        if (token_stack->items[s_top].code > T_BOUND) {
            j = s_top - 1;
        } else {
            j = s_top;
        }
        
        /* 如果栈内算符优先级大于串中优先级，进行归约 */
        while (token_pri[token_stack->items[j].code-1][ta.code-1] == GRE) {
            
            /* 对常数进行归约：N <= c */
            if (token_stack->items[j].code == T_CONST) {
                token_stack->items[j].code = T_N;
            }
            
            /* 对变量进行归约: N <= i */
            if (token_stack->items[j].code == T_VAR) {
                int re;
                re = var_reduction(&(token_stack->items[j]), dict);
                if (re == VAR_NOT_DEFINE) {
                    printf("NameError: name '%s' is not defined!\n",
                            (char *)token_stack->items[j].value);
                    return;
                }
            }

            /* 对括号进行归约：N <= (N) */
            if (token_stack->items[j].code == T_BRACES_R) {
                token_stack->items[j-2].code = T_N;
                token_stack->items[j-2].value = token_stack->items[j-1].value;
                j = j - 2;
            }

            /* 对打印进行归约：N <= ?N */
            if (token_stack->items[j].code == T_PRINT) {
                printf(PRINT_DOUBLE_FORMT, 
                           *(GakioNum *)(token_stack->items[j+1].value));  
            }

            /* 赋值归约：N <= i = N */
            if (token_stack->items[j].code == T_ASSING) {
                
                /* 如果 = 右边不是 N，则出现语法错误 */
                if (token_stack->items[j+1].code != T_N)  {
                    printf("SyntaxError: '=' must be like 'v = N'!\n");
                    return;
                }
                value_reduction(&(token_stack->items[j-1]), &(token_stack->items[j+1]), dict);
                j = j - 1;
            }

            /* 对算术进行归约：N <= N +|-|*|/ N */
            if ((token_stack->items[j].code <= T_DIVI) && (token_stack->items[j].code >= T_PLUS)) {
                int reg;
                reg = arith_reduction(&(token_stack->items[j-1]), &(token_stack->items[j+1]),
                                      &(token_stack->items[j]), vpool);
                if (reg == LACK_OPER) {
                    char op[3];
                    code_to_op_string(token_stack->items[j].code, op);
                    printf("ArithmeticError: '%s' missing operand!\n", op);
                    return;    
                }
                if (reg == DIVZERO) {
                    printf("ZeroDivisionError: division by zero!\n");
                    return;
                }

                /* reg == 0，则正常结束，此时将左操作数换为归约后的 N，栈顶指针下移一位；
                 * reg == 1，则出现负数处理，此时将操作符换为归约后的 N，栈顶指针不变
                 */
                if (reg == 0) {
                     j = j - 1;
                }
            }

            /* 归约后，向下扫描，找到后一个终结符 */
            do {
                    j = j - 1;
                } while (token_stack->items[j].code > T_BOUND);
            
            /* 归约后，调整栈使用大小，此时 j 总是指向栈顶下的第2个元素 */
            token_stack->use = j + 2;
        }

        if (token_pri[token_stack->items[j].code-1][ta.code-1] == LE
            || token_pri[token_stack->items[j].code-1][ta.code-1] == EQU) {
            
            ta.value = token_array->items[idx].value;
            token_adt_append(token_stack, &ta);
        } else {
            printf("SyntaxError: '%s'!\n",
                    (char *)token_stack->items[j].value);
            return;
        }

        idx++;
    }
}
