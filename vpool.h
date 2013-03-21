#ifndef _VPOOL_H_
#define _VPOOL_H_


typedef struct {
    unsigned long size;
    unsigned long use;
    void **values;
} valuepool;


/* 创建类似数组的 token 结构，其中每一个元素是一个 token 二元组
 * 
 * @size token 串最大容量
 *
 * return 指向结构的指针
 */
valuepool *value_pool_new();

/* 删除 token 串 */
void value_pool_delete(valuepool *vpool);

/* 添加 token 进 token 串
 * 
 * @tokens 指向 token 串的指针
 * @item token 二元组
 *
 * return 0 添加失败
 * return 1 添加成功
 */
int value_pool_append(valuepool *vpool, void *value);

void *value_pool_find(valuepool *vpool, void *value);

#endif