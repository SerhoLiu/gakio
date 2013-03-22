#include <stdio.h>
#include <string.h>
#include "tokenadt.h"
#include "numdict.h"
#include "vpool.h"
#include "lex.h"
#include "parser.h"
#include "gakio.h"

#define GAKIO_REPL_INFO "Hello..." GAKIO_VERSION "\nType ':q' quit this REPL\n"

int main(int argc, char const *argv[])
{
    tokenadt *array, *stack;
    numdict *dict;
    valuepool *vpool;
    
    array = token_adt_new(MAXADTLEN);
    stack = token_adt_new(MAXADTLEN);
    dict = numdict_new();
    vpool = value_pool_new();

    char b[MAXINPUT];
    int l;
    const char *info = GAKIO_REPL_INFO;
    fputs(info, stdout);
    fflush(stdout);

    const char *prmt = ">> ";
    while (1) {
        fputs(prmt, stdout);
        fflush(stdout);

        if (fgets(b, MAXINPUT, stdin) == NULL)
                return 0; 
        l = strlen(b);

        /* 如果输入字符长度大于 MAXINPUT，将截取 MAXINPUT 长的字符串;
         * 并清空标准输入中剩余的字符
         */
        if (l > 0 && b[l-1] != '\n') {
            b[l-1] = '\n';
            while (getchar() != '\n');
        }

        if (l > 0 && b[l-1] == '\n')  
            b[l-1] = '\0'; 

        if (strcmp(b, ":q") != 0) {
            
            l = akio_lex(array, vpool, b);
            if (!l) {
                reduction(array, stack, dict, vpool);
            }
            token_adt_reset(array);
            token_adt_reset(stack);
        } else {
            break;
        }
    }

    token_adt_delete(array);
    token_adt_delete(stack);
    numdict_delete(dict);
    value_pool_delete(vpool);
    return 0;
}