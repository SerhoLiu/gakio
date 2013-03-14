/*
 * Hash 表，采用开放寻址法，冲突解决方法是双重散列
 * 表容量固定，当元素个数大于容量时，会出错退出
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "numdict.h"


typedef struct {
    char *key;                      
    void *value;                    
} dictitem;


struct numdict {
    unsigned int size;
    unsigned int use;
    unsigned int lookup;  /* for test */
    dictitem *items;
};


static unsigned int string_hash(const char *str);


numdict * numdict_new(unsigned int size)
{
    numdict *dict;
    
    dict = malloc(sizeof(numdict));
    if (dict == NULL) {
        return NULL;
    }
    dict->size = size;
    dict->use = 0;
    dict->lookup = 0;
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


int numdict_put(numdict *dict, const char *key, void * const value)
{
    int key_len;
    key_len = strlen(key);
    
    dictitem *item;

    if (dict == NULL) {
        printf("dict is NULL!\n");
        return 0;
    }
    if (key == NULL || value == NULL) {
        printf("key is NULL or value is NULL\n");
        return 0;
    }
    assert(dict->size > dict->use);

    
    unsigned int hash = string_hash(key);
    unsigned int index = hash;
    unsigned int perturb = hash;
    
    while (1) {
        index = index & (dict->size - 1);
        item = &(dict->items[index]);
        
        dict->lookup++;  /* for test */

        /* 找到相同的 key, 替换值 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            item->value = value;
            printf("key in, repce!\n");
            return 1;
        }

        if (item->key == NULL) {
            item->key = (char *)malloc((key_len + 1) * sizeof(char));
            if (item->key == NULL) {
                return 0;
            }
            strcpy(item->key, key);
            item->value = value;
            dict->use++;
            printf("append new key index is %d!\n", index);
            return 1;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }
}

void *numdict_get(const numdict *dict, const char *key)
{
    dictitem *item;

    if (dict == NULL) {
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }

    unsigned int hash = string_hash(key);
    unsigned int index = hash;
    unsigned int perturb = hash;

    while (1) {
        index = index & (dict->size - 1);
        item = &(dict->items[index]);
        
        if (item->key == NULL) {
            return NULL;
        }

        /* key 相同，查找成功 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            return item->value;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }

}

unsigned int get_lookup(numdict *dict)
{
    return dict->lookup;
}


/* 这个 Hash 函数来自于 Python */
static unsigned int string_hash(const char *str) {
    unsigned int x;
    unsigned int len = 0;
 
    x = *str << 7;
    while (*str) {
        x = (100003 * x) ^ *str++;
        len++;
    }
    x ^= len;
    return x;
}
