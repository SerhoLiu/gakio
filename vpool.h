#ifndef _VPOOL_H_
#define _VPOOL_H_

typedef struct {
    unsigned long size;
    unsigned long use;
    void **values;
} valuepool;


/* 创建类似数组的 valuepool 结构，其中每一个元素是一个指向
 * 对象的指针
 *
 * return 指向结构的指针
 */
valuepool *value_pool_new();

/* 删除 vpool，并释放所有对象的空间 */
void value_pool_delete(valuepool *vpool);

/* 添加 对象指针 进 vpool
 * 
 * @vpool 指向 valuepool 串的指针
 * @value 指向对象的指针
 *
 * return 0 添加失败
 * return 1 添加成功
 */
int value_pool_append(valuepool *vpool, void *value);

/* 在 value pool 里查找对象，适合采用引用的对象，比如字符串
 * 
 * @vpool 指向 valuepool 串的指针
 * @value 指向对象的指针
 *
 * return 如果查找成功，返回指向该对象的指针，否则 NULL
 */
void *value_pool_find(valuepool *vpool, const void *value);

#endif