#ifndef _FAKIO_H_
#define _FAKIO_H_

/* 
 * 对于数值，支持常整型和双精度浮点型，这里采用 Tagged Pointer
 * 方法，将指向 double 类型值的指针的值末尾变成1,以便和指向 long
 * 的指针向区分
 * 可以参考：http://serholiu.com/talk-tagged-pointer
 *
 */
#define IS_DOUBLE(p) (((unsigned long) (p) & 0x1) != 0)
#define MAKE_DOUBLE(p) ((void *)((unsigned long) (p) | 0x1))
#define GET_DOUBLE(p) ((unsigned long) (p) & ~0x1)
