#include <stdlib.h>
#include <string.h>
#include "vpool.h"
#include "gakio.h"

#define VPOOL_INIT_SIZE (1 << 9)
#define VPOOL_RESIZE_LACK (1 << 2)

static valuepool *_value_pool_new(unsigned long size);
static void value_pool_resize(valuepool *vpool);

valuepool *value_pool_new()
{
    return _value_pool_new(VPOOL_INIT_SIZE);
}


void value_pool_delete(valuepool *vpool)
{
    unsigned long use, i;
    use = vpool->use;

    void **values = vpool->values;
    for (i = 0; i < use; i++) {
        if (IS_GAKIONUM(values[i])) {
            free((void *)(GET_GAKIONUM(values[i])));
        } else {
            free(values[i]);
        }
    }

    free(vpool->values);
    free(vpool);
}

int value_pool_append(valuepool *vpool, void *value)
{
    if (vpool == NULL) {
        return 0;
    }
    if (vpool->use + VPOOL_RESIZE_LACK > vpool->size) {
        value_pool_resize(vpool);
    }
    vpool->values[vpool->use] = value;
    vpool->use++;
    return 1;
}

void *value_pool_find(valuepool *vpool, void *value)
{
    unsigned long use, i;
    use = vpool->use;

    void **values = vpool->values;
    for (i = 0; i < use; i++) {
        if (IS_GAKIONUM(value)) {
            if (*(GakioNum *)(GET_GAKIONUM(values[i])) == (*(GakioNum *)(GET_GAKIONUM(value)))) {
                return (void *)(GET_GAKIONUM(values[i]));
            }
        } else {
            if (!strcmp((char *)values[i], (char *)value)) {
                return values[i];
            }
        }
    }
    return NULL;
}

static valuepool *_value_pool_new(unsigned long size)
{
    valuepool *vpool;
    vpool = (valuepool *)malloc(sizeof(valuepool));
    if (vpool == NULL) {
        return NULL;
    }

    vpool->size = size;
    vpool->use = 0;
    vpool->values = malloc(size * sizeof(void*));
    if (vpool->values == NULL) {
        free(vpool);
        return NULL;
    }
    memset(vpool->values, 0, size * sizeof(void*)); 
    return vpool;
}


static void value_pool_resize(valuepool *vpool)
{
    unsigned long newsize, oldsize, olduse, i;
    void **newvalues, **oldvalues;

    oldsize = vpool->size;
    newsize = oldsize << 1;
    olduse = vpool->use;
    oldvalues = vpool->values;

    //printf("oldsize = %ld; olduse = %ld\n", oldsize, olduse);

    newvalues = malloc(newsize * sizeof(void*));
    if (newvalues == NULL) {
        return;
    }

    for (i = 0; i < olduse; i++) {
        newvalues[i] = oldvalues[i];
    }

    vpool->values = newvalues;
    vpool->size = newsize;
    //printf("FREE OLDVALUES\n");
    free(oldvalues);
}