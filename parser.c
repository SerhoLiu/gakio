#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define TOKENNUM 11
#define GRE 1
#define EQU 0
#define LE -1
#define NONE -2

#define VAR_NOT_DEFINE 720
#define INVALID_OP 721
#define LACK_OPER 722
#define DIVZERO 723

static const int token_pri[TOKENNUM][TOKENNUM] = {
    /* +   -    *   /  (    )   i   c   ?     =      #  */
    {GRE, GRE, LE, LE, LE, GRE, LE, LE, NONE, NONE, GRE},          // +
    {GRE, GRE, LE, LE, LE, GRE, LE, LE, NONE, NONE, GRE},          // -
    {GRE, GRE, GRE, GRE, LE, GRE, LE, LE, NONE, NONE, GRE},        // *
    {GRE, GRE, GRE, GRE, LE, GRE, LE, LE, NONE, NONE, GRE},        // /
    {LE, LE, LE, LE, LE, EQU, LE, LE, NONE, NONE, NONE},           // (
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, NONE, GRE},  // )
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, LE, GRE},   // i
    {GRE, GRE, GRE, GRE, NONE, GRE, NONE, NONE, NONE, NONE, GRE},  // c
    {LE, LE, LE, LE, LE, NONE, LE, LE, NONE, NONE, GRE},           // ?
    {LE, LE, LE, LE, LE, NONE, LE, LE, NONE, NONE, GRE},            // =
    {LE, LE, LE, LE, LE, NONE, LE, LE, LE, LE, EQU}                // #
};


static int ch_in_str(const char ch, const char *str)
{
    do { 
        if (ch == *str)
            return 1;
    } while (str++ && (*str != '\0'));

    return 0;
}


/* N <= i
 * @t 变量 i
 * @dict 变量表
 *
 * return VAR_NOT_DEFINE 变量 i 没有定义
 * return 0 成功
 */
static int var_reduction(token *t, numdict *dict)
{
    int i;
    itemvalue item;
    i = numdict_get(dict, t->value, &item);
    if (i == 0) {
        return VAR_NOT_DEFINE;
    }
    if (item.v_type == LONG) {
        sprintf(t->value, "%ld", item.num.v_long);
    } else {
        sprintf(t->value, "%f", item.num.v_double);
    }
    t->code = 12;
    return 0;
}


/* 
 * N <= N +|-|*|/ N 
 * @l 左操作数
 * @r 右操作数
 * @op 操作符
 * 结果将修改 l或者op 作为返回值
 *
 * return LACK_OPER 缺少操作数
 * return DIVZERO   除 0 错误
 * return 1 - 符缺少左操作数，以负数处理
 * return 0 正常
 */
static int arith_reduction(token *l, token *r, token *op)
{
    long dl, dr, dre;
    dl = dr = dre = 0;
    double fl, fr, fre;
    fl = fr = fre = 0;
    int il, ir, type = DOUBLE, neg = 0;
    char left[20];
    strcpy(left, l->value);

    /* 无左操作数错误处理 */
    if (l->code != 12) {

        /* 负数处理：如果是 -,添 0 */
        if (op->code == 2) {
            strcpy(left, "0");
            neg = 1;
        } else {
            return LACK_OPER;
        } 
    }

    /* 无右操作数错误处理 */
    if (r->code != 12) {
        return LACK_OPER;
    }

    il = ch_in_str('.', left);
    ir = ch_in_str('.', r->value);
    
    if(il || ir) {
        fl = atof(left);
        fr = atof(r->value);
    } else {
        dl = atol(left);
        dr = atol(r->value);
        type = LONG;
    }

    switch (op->code) {
        case 1: {
            if (type == LONG) {
                dre = dl + dr;
            } else {
                fre = fl + fr;
            }
            break;
        }
        case 2: {
            if (type == LONG) {
                dre = dl - dr;
            } else {
                fre = fl - fr;
            }
            break;
        }
        case 3: {
            if (type == LONG) {
                dre = dl * dr;
            } else {
                fre = fl * fr;
            }
            break;
        }
        case 4: {
            if (type == LONG) {
                if (dr == 0) {
                    return DIVZERO;
                }
                dre = dl / dr;
            } else {
                if (fr == 0) {
                    return DIVZERO;
                }
                fre = fl / fr;
            }
            break;
        }
        default:  { return INVALID_OP;}
    }
    if (neg) {
        op->code = 12;
        if (type == LONG) {
            sprintf(op->value, "%ld", dre);
        } else {
            sprintf(op->value, "%f", fre);
        }
        return neg;
    } else {
        l->code = 12;
        if (type == LONG) {
            sprintf(l->value, "%ld", dre);
        } else {
            sprintf(l->value, "%f", fre);
        }
    }
    return 0;
}


/*
 * N <= i = N 
 * @i 变量
 * @n N
 * @dict 变量表
 */
static void value_reduction(token *i, token *n, numdict *dict)
{
    itemvalue item;
    if (ch_in_str('.', n->value)) {
        item.v_type = DOUBLE;
        item.num.v_double = atof(n->value);
    } else {
        item.v_type = LONG;
        item.num.v_long = atol(n->value);
    }
    numdict_put(dict, i->value, &item);
    n->code = 12;
}


void reduction(tokenadt *token_array, tokenadt *token_stack, numdict *dict)
{ 
    token ta, ts;
    
    /* 将 # 入栈 */
    ts.code = 11;
    strcpy(ts.value, "#");
    token_adt_append(token_stack, &ts);
    
    size_t idx = 0;
    size_t s_top, j;
    while (idx < token_array->use) {
        ta.code = token_array->items[idx].code;
        
        s_top = token_stack->use - 1;

        /* 判断栈顶元素是否在符号表中 */         
        if (token_stack->items[s_top].code > 11) {
            j = s_top - 1;
        } else {
            j = s_top;
        }
        
        /* 如果栈内算符优先级大于串中优先级，进行归约 */
        while (token_pri[token_stack->items[j].code-1][ta.code-1] == GRE) {
            
            /* 对常数进行归约：N <= c */
            if (token_stack->items[j].code == 8) {
                token_stack->items[j].code = 12;
            }
            
            /* 对变量进行归约: N <= i */
            if (token_stack->items[j].code == 7) {
                int re;
                re = var_reduction(&(token_stack->items[j]), dict);
                if (re == VAR_NOT_DEFINE) {
                    printf("NameError: name '%s' is not defined!\n", token_stack->items[j].value);
                    return;
                }
            }

            /* 对括号进行归约：N <= (N) */
            if (token_stack->items[j].code == 6) {
                token_stack->items[j-2].code = 12;
                strcpy(token_stack->items[j-2].value, token_stack->items[j-1].value);
                j = j - 2;
            }

            /* 对打印进行归约：N <= ?N */
            if (token_stack->items[j].code == 9) {
                printf("%s\n", token_stack->items[j+1].value);
            }

            /* 赋值归约：N <= i = N */
            if (token_stack->items[j].code == 10) {
                
                /* 如果 = 右边不是 N，则出现语法错误 */
                if (token_stack->items[j+1].code != 12)  {
                    printf("SyntaxError: '=' must be like 'v = N'!\n");
                    return;
                }
                value_reduction(&(token_stack->items[j-1]), &(token_stack->items[j+1]), dict);
                j = j - 1;
            }

            /* 对算术进行归约：N <= N +|-|*|/ N */
            if ((token_stack->items[j].code < 5) && (token_stack->items[j].code > 0)) {
                int reg;
                reg = arith_reduction(&(token_stack->items[j-1]), &(token_stack->items[j+1]),
                                      &(token_stack->items[j]));
                if (reg == LACK_OPER) {
                    printf("ArithmeticError: '%s' missing operand!\n", token_stack->items[j].value);
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
                } while (token_stack->items[j].code > 11);
            
            /* 归约后，调整栈使用大小，此时 j 总是指向栈顶下的第2个元素 */
            token_stack->use = j + 2;
        }

        if (token_pri[token_stack->items[j].code-1][ta.code-1] == LE
            || token_pri[token_stack->items[j].code-1][ta.code-1] == EQU) {
            
            strcpy(ta.value, token_array->items[idx].value);
            token_adt_append(token_stack, &ta);
        }else {
            printf("SyntaxError: '%s'!\n", token_stack->items[j].value);
            return;
        }

        idx++;
    }
}
