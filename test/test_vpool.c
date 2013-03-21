#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vpool.h"
#include "../gakio.h"
#include "minunit.h"

valuepool *vpool;
char test[100] = "HHHHHHH";

char *test_new_vpool()
{
    vpool = value_pool_new();
    mu_assert(vpool != NULL, "new vpool failed!");
    return NULL;
}

char *test_vpool_append()
{
    GakioNum *num;
    num = (GakioNum *)malloc(sizeof(GakioNum));
    //debug("#### the put &VLONG: %p", &VLONG);
    *num = 12.9;
    num = MAKE_GAKIONUM(num);
    int r = value_pool_append(vpool, num);
    mu_assert(r == 1, "append GakioNum to vpool failed!" );

    char *va = (char *)malloc(sizeof(char) * (strlen(test) + 1));
    strcpy(va, test);
    r = value_pool_append(vpool, va);
    mu_assert(r == 1, "append string to vpool failed!" );
    return NULL;
}

char *test_vpool_resize()
{
    GakioNum *num;
    GakioNum i = 0;
    
    //int r;
    for (i = 0; i < (1 << 12); i++) {
        num = (GakioNum *)malloc(sizeof(GakioNum));
        *num = i;
        num = MAKE_GAKIONUM(num);
        value_pool_append(vpool, num);
        debug("new append %lf", i);
    }
    mu_assert(i == (1 << 12), "vpool resize failed!" );
    return NULL;
}

char *test_vpool_find()
{
    GakioNum *tnum, num = 12.9;
    tnum = &num;
    tnum = MAKE_GAKIONUM(tnum);
    void *r = value_pool_find(vpool, tnum);
    mu_assert(*(GakioNum *)(GET_GAKIONUM(r)) == num, "find GakioNum from vpool failed!");

    r = value_pool_find(vpool, test);
    debug("#### find string form vpool %s", (char *)r);
    return NULL;
}

char *test_delete_vpool()
{
    void *p = (void *)vpool;
    value_pool_delete(vpool);
    mu_assert(vpool == p, "delete numdict failed!")
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_new_vpool);
    mu_run_test(test_vpool_append);
    mu_run_test(test_vpool_resize);
    mu_run_test(test_vpool_find);
    mu_run_test(test_delete_vpool);
  
    return NULL;
 }
  
RUN_TESTS(all_tests);