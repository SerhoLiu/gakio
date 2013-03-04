/*
 * Hash 表，采用开放寻址法，冲突解决方法是双重散列
 * 表容量固定，当元素个数大于容量时，会出错退出
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "numdict.h"

#define FIRST_HASH(KEY, SIZE)  (KEY % SIZE)
#define SECOND_HASH(KEY, SIZE) (1 + (KEY % (SIZE - 1)))

#define RIGHT_TO_LEFT(left, right) do { \
    (left)->v_type = (right)->v_type; \
    (right)->v_type == LONG ? ((left)->num.v_long = (right)->num.v_long) :   \
                              ((left)->num.v_double = (right)->num.v_double); \
} while(0)


typedef struct {
    char *key;                      
    int v_type;
    numbers num;                     
} dictitem;


struct numdict {
    unsigned int size;
    unsigned int use;
    dictitem *items;
};


static unsigned int bkdr_hash(const char *str);

numdict * numdict_new(unsigned int size)
{
    numdict *dict;
    
    dict = malloc(sizeof(numdict));
    if (dict == NULL) {
        return NULL;
    }
    dict->size = size;
    dict->use = 0;
    dict->items = (dictitem *)malloc(dict->size * sizeof(dictitem));
    if (dict->items == NULL) {
        free(dict);
        return NULL;
    }
    memset(dict->items, 0, dict->size * sizeof(dictitem)); 
    return dict;
}

void numdict_delete(numdict *dict)
{
    int n, i;
    n = i = 0;
    dictitem *items = NULL;

    if (dict == NULL) {
        return;
    }
    n = dict->size;
    
    items = dict->items;   /* free dictitem key string */
    while (i < n) {
        free(items->key);
        items++;
        i++;
    }

    free(dict->items);    /* free all dictitem */
    free(dict);           /* free dict node */
}

int numdict_put(numdict *dict, const char *key, const itemvalue *value)
{
    unsigned int hash, f_hash, s_hash, index;
    hash = f_hash = s_hash = index = 0;
    
    int key_len, i = 0;
    key_len = strlen(key);
    
    dictitem *item;

    if (dict == NULL) {
        return 0;
    }
    if (key == NULL || value == NULL) {
        return 0;
    }
    assert(dict->size > dict->use);

    hash = bkdr_hash(key);
    f_hash = FIRST_HASH(hash, dict->size);
    s_hash = SECOND_HASH(hash, dict->size);

    while (1) {
        index = (f_hash + i * s_hash) % (dict->size);
        item = &(dict->items[index]);

        /* 找到相同的 key, 替换值 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            RIGHT_TO_LEFT(item, value);
            return 1;
        }

        if (item->key == NULL) {
            item->key = (char *)malloc((key_len + 1) * sizeof(char));
            if (item->key == NULL) {
                return 0;
            }
            strcpy(item->key, key);
            RIGHT_TO_LEFT(item, value);
            dict->use++;
            return 1;
        }

        i++;
    }
}

int numdict_get(const numdict *dict, const char *key, itemvalue *value)
{
    unsigned int hash, f_hash, s_hash, index;
    hash = f_hash = s_hash = index = 0;
    
    int i = 0;
    
    dictitem *item;

    if (dict == NULL) {
        return 0;
    }
    if (key == NULL) {
        return 0;
    }

    hash = bkdr_hash(key);
    f_hash = FIRST_HASH(hash, dict->size);
    s_hash = SECOND_HASH(hash, dict->size);

    while (1) {
        index = (f_hash + i * s_hash) % (dict->size);
        item = &(dict->items[index]);
        
        if (item->key == NULL) {
            return 0;
        }

        /* key 相同，查找成功 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            RIGHT_TO_LEFT(value, item);
            return 1;
        }

        i++;
    }

}


static unsigned int bkdr_hash(const char *str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
 
    while (*str)
    {
        hash = hash*seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}
