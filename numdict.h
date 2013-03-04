#ifndef _NUMDICT_H_
#define _NUMDICT_H_

#define LONG 1
#define DOUBLE 2

/* 存放变量值的联合
 * @v_long long
 * @v_double double
 */
typedef union {
    long v_long;
    double v_double;
} numbers;


/* 变量结构
 * @v_type {LONG, DOUBLE} 变量类型
 * @num 变量值
 */
typedef struct {
    int v_type;
    numbers num;    
} itemvalue;

/* 变量表结构 */
typedef struct numdict numdict;

/* 新建表量表
 *
 * @size 表容量大小
 *
 * return 指向表的指针
 */
numdict * numdict_new(unsigned int size);


/* 删除变量表 */
void numdict_delete(numdict *dict);


/* 添加变量到变量表
 *
 * @dict 指向变量表的指针
 * @key 变量名
 * @value 变量的值
 *
 * return 1 添加成功
 * return 0 添加失败
 */
int numdict_put(numdict *dict, const char *key, const itemvalue *value);


/* 变量表中读取变量的值
 *
 * @dict 指向变量表的指针
 * @key 变量名
 * @value 变量的值，作为返回值
 *
 * return 1 查找失败
 * return 0 查找成功
 */
int numdict_get(const numdict *dict, const char *key, itemvalue *value);

#endif 