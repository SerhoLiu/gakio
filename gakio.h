#ifndef _GAKIO_H_
#define _GAKIO_H_

/* 表达式串最大字符数 */
#define MAXINPUT 2048

/* token 串，归约栈，最大容量 */
#define MAXADTLEN 131

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


/* 
 * 对于数值，支持常整型和双精度浮点型，这里采用 Tagged Pointer
 * 方法，将指向 double 类型值的指针的值末尾变成1,以便和指向 long
 * 的指针向区分
 * 可以参考：http://serholiu.com/talk-tagged-pointer
 *
 */
#define GakioNum double
#define PRINT_DOUBLE_FORMT "%.8lf\n"
#define PRINT_INT_FORMT "%.0lf\n"

#define IS_VARIABLE(p) (((unsigned long) (p) & 0x1) != 0)
#define MAKE_VARIABLE(p) ((void *)((unsigned long) (p) | 0x1))
#define GET_VARIABLE(p) ((unsigned long) (p) & ~0x1)

#define IS_GAKIONUM(p) (((unsigned long) (p) & 0x1) != 0)
#define MAKE_GAKIONUM(p) ((void *)((unsigned long) (p) | 0x1))
#define GET_GAKIONUM(p) ((unsigned long) (p) & ~0x1)

#endif
