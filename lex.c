#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"
#include "gakio.h"

#define MAXINPUT 1024
#define INVALID_SYNTAX 717
#define INVALID_TOKEN  718
#define INVALID_PRINT  719

/*
 *tokens code
 *-----------------------------------------------
 * + | - | * | / | ( | ) | i | c | ? | = | # | N
 *-----------------------------------------------
 * 1   2   3   4   5   6   7   8   9   10  11  12
 */ 

static char OPT_STR[10] = "+-*/()";

static void remove_str_space(char *str)
{
    char temp[MAXINPUT];
    size_t i = 0, j = 0;

    while (*(str++) != '\0') {
        if (*str != ' ') {
            *(temp+i) = *str;
            i++;
        }
        j++;
    }
    *(temp + i) = '\0';

    strcpy(str-j, temp);
}

static int ch_in_str(const char ch, const char *str)
{
    do { 
        if (ch == *str)
            return 1;
    } while (str++ && (*str != '\0'));

    return 0;
}

/*
 * @str 预处理后的表达式字符串
 * @value 作为返回值，token 单元
 * @index 处理索引，指向上次处理的位置
 *
 * return index 此次处理结束后的位置
 * return INVALID_SYNTAX, INVALID_PRINT, INVALID_TOKEN 词法错误
 */
static int get_next_op(const char str[], char *value, int index)
{
    int i = 0, ch;
    
    ch = str[index];
    
    /* 如果是字母或者下划线，开始判断是否是变量名 */
    if (isalpha(ch) || ch == '_') {
        value[i] = ch;
        while ((ch = str[++index]) && (isalpha(ch) || isdigit(ch) || (ch == '_') || (ch == '.'))) {
                value[++i] = ch;
        }
        value[++i] = '\0';
        
        /* 如果变量名中含有.，则错误 */
        if (ch_in_str('.', value)) {
            return INVALID_SYNTAX;
        } else {
            return index;
        }  
    }

    /* 如果是数字，开始判断是否是常数 */
    if (isdigit(ch)) {
        value[i] = ch;
        while((ch = str[++index]) && (!ch_in_str(ch, OPT_STR))) {

            /* 如果数字后面有字母等，则是非法的变量名 */
            if (!isdigit(ch) && (!ch_in_str(ch, OPT_STR) && (ch != '.'))) {
                value[++i] = ch;
                value[++i] = '\0';
                return INVALID_SYNTAX;
            }
            else if (isdigit(ch) || ch == '.') {
                value[++i] = ch;
            }
        }
        value[++i] = '\0';
        return index;
    }

    /* 如果是 ?，则必须是表达式的第一个字符 */
    if (ch == '?') {         
        if (index != 0) { 
            value[i] = ch;
            value[++i] = '\0';
            return INVALID_PRINT;
        }
    }

    switch (ch) {
        case '+': {value[i] = '+'; value[++i] = '\0'; break;}
        case '-': {value[i] = '-'; value[++i] = '\0'; break;}
        case '*': {value[i] = '*'; value[++i] = '\0'; break;}
        case '/': {value[i] = '/'; value[++i] = '\0'; break;}
        case '(': {value[i] = '('; value[++i] = '\0'; break;}
        case ')': {value[i] = ')'; value[++i] = '\0'; break;}
        case '?': {value[i] = '?'; value[++i] = '\0'; break;}
        case '=': {value[i] = '='; value[++i] = '\0'; break;}
        default:  {value[i] = ch; value[++i] = '\0'; return INVALID_TOKEN;}  //含有其它非法字符
    }
    index++;
    return index;
}

/*
 * 生成 token 串，以二元组的形式存放在 tokens 指针指向的队列
 * @tokens token 串指针
 * @value token 单元
 */
static void create_tokens_array(tokenadt *tokens, const char *value)
{
    char ch;
    token t;

    if (tokens == NULL) {
        return;
    }

    ch = value[0];
    if (isdigit(ch)) {
        t.code = 8;
        /* 建立数值，使用 malloc，因为使用指针指向 */
        //strcpy(t.value, value);
        if (ch_in_str('.', value)) {
            long *vlong = malloc(sizeof(long));
            *vlong = atol(value);
            t.value = vlong;
        } else {
            double *vdouble = malloc(sizeof(double));
            *vdouble = atof(value);
            t.value = MAKE_DOUBLE(vdouble);
        }
        token_adt_append(tokens, &t);
    }
    else if (isalpha(ch) || ch == '_') {
        t.code = 7;
        char *str = malloc((strlen(value) + 1) * sizeof(char));
        strcpy(str, value);
        t.value = str;
        token_adt_append(tokens, &t);
    }

    switch (ch) {
        case '+': {t.code = 1; break;}
        case '-': {t.code = 2; break;}
        case '*': {t.code = 3; break;}
        case '/': {t.code = 4; break;}
        case '(': {t.code = 5; break;}
        case ')': {t.code = 6; break;}
        case '?': {t.code = 9; break;}
        case '=': {t.code = 10; break;}
        default:  {return;}
    }
    //strcpy(t.value, value);
    t.value = NULL;
    token_adt_append(tokens, &t);
}


int akio_lex(tokenadt *tokens, char *codes)
{
    remove_str_space(codes);
            
    int index = 0, len_str;
    char value[20];
    len_str = strlen(codes);
    
    while ((index != INVALID_TOKEN) && (index != INVALID_SYNTAX) && 
           (index != INVALID_PRINT) && (index < len_str)) {
            
        index = get_next_op(codes, value, index);

        if (index == INVALID_SYNTAX) {
            printf("SyntaxError: invalid syntax \"%s\"\n", value);
            return 1;
        }
        else if (index == INVALID_TOKEN) {
            printf("SyntaxError: invalid token \"%s\"\n", value);
            return 1;
        }
        else if (index == INVALID_PRINT) {
            printf("SyntaxError: ? must a first \"%s\"\n", value);
            return 1;
        } else {
            create_tokens_array(tokens, value);
        }   
    }

    /* 在 token 串后添加 #字符 */
    token t;
    t.code = 11;
    //strcpy(t.value, "#");
    t.value = NULL;
    token_adt_append(tokens, &t);
    return 0; 
}
