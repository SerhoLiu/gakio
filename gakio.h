#ifndef _GAKIO_H_
#define _GAKIO_H_

#define GAKIO_VERSION "Gakio 1.1.0 By SErHo"

/* 表达式串最大字符数 */
#define MAXINPUT 2048

/* token 串，归约栈，最大容量 */
#define MAXADTLEN 1024

/* 标识符最大长度 */
#define MAXTOKEN 128

/* 异常信息代码 */
#define INVALID_SYNTAX 717
#define INVALID_TOKEN  718
#define INVALID_PRINT  719
#define VAR_NOT_DEFINE 720
#define INVALID_OP 721
#define LACK_OPER 722
#define DIVZERO 723


/* 一切数值都是 double 类型，方便处理 */
#define GakioNum double
#define PRINT_DOUBLE_FORMT "%.10g\n"


/* 
 * 在 vpool 中，存储指向字符串和数值的指针，这里使用 Tagged Pointer 
 * 方法，将指向 GakioNum 类型值的指针的值末尾变成1, 以便区别指向 string
 * 的指针
 * 可以参考：http://serholiu.com/talk-tagged-pointer
 */
#define IS_GAKIONUM(p) (((unsigned long) (p) & 0x1) != 0)
#define MAKE_GAKIONUM(p) ((void *)((unsigned long) (p) | 0x1))
#define GET_GAKIONUM(p) ((unsigned long) (p) & ~0x1)

#endif
