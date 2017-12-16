/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <yunit.h>
#include <yts.h>
#include "hashtable.h"

typedef struct {
    char name[10];
    char addr[20];
    char phone[20];
} stu_user_info;

static void *g_ht = NULL;
static int   g_ht_size = 5;

static int key_integer = 5;
static int val_integer = 10;
static const char *key_str = "user_name";
static const char *val_str = "tiger";
static const char *key_str_2 = "user_info";
static stu_user_info val_stu = {"xxxxx", "yyyyyyyy", "zzzzzzzzzzz"};

static void test_ht_add(void)
{
    int ret = 0;
    int i;

    for (i=0;i<g_ht_size*2;i++) {
        ret = ht_add(g_ht, &i, sizeof(int *), &i, sizeof(int));
        YUNIT_ASSERT(0 == ret);
    }
    
    for (i=0;i<g_ht_size*2;i++) {
        int tmp, len;
        len = sizeof(int);
        ht_find(g_ht, &i, sizeof(int *), &tmp, &len);
        YUNIT_ASSERT(i == tmp && len == sizeof(i));
        ret = ht_del(g_ht, &i, sizeof(i));
        YUNIT_ASSERT(0 == ret);
    }

    ret = ht_add(g_ht, &key_integer, sizeof(key_integer), &val_integer, sizeof(int));
    YUNIT_ASSERT(0 == ret);

    ret = ht_add(g_ht, &key_integer, sizeof(key_integer), &val_integer, sizeof(int));
    YUNIT_ASSERT(0 == ret);

    ret = ht_add(g_ht, key_str, strlen(key_str) + 1, val_str, strlen(val_str) + 1);
    YUNIT_ASSERT(0 == ret);

    ret = ht_add(g_ht, key_str_2, strlen(key_str_2) + 1, &val_stu, sizeof(val_stu));
    YUNIT_ASSERT(0 == ret); 
}

static void test_ht_find(void)
{
    int tmp = 0;
    int len = 0;
    len = sizeof(tmp);
    ht_find(g_ht, &key_integer, sizeof(&key_integer), &tmp, &len);

    YUNIT_ASSERT(tmp == val_integer);

    char tmp_buff[20] = {0};
    len = sizeof(tmp_buff);
    ht_find(g_ht, key_str, strlen(key_str) + 1, tmp_buff, &len);
    YUNIT_ASSERT(0 == strcmp(tmp_buff, val_str));
    YUNIT_ASSERT(len == strlen(val_str)+1);

    stu_user_info tmp_user ;
    len = sizeof(tmp_user);
    ht_find(g_ht, key_str_2, strlen(key_str_2) + 1, &tmp_user, &len);
    YUNIT_ASSERT(0 == strcmp(tmp_user.name, val_stu.name) || strcmp(tmp_user.phone, val_stu.phone));
}

static void test_ht_del(void)
{
    int ret = 0;
    char tmp_buff[20] = {0};
    int len = 0;

    len = sizeof(tmp_buff);
    ht_find(g_ht, key_str, strlen(key_str) + 1, tmp_buff, &len);
    YUNIT_ASSERT(0 == strcmp(tmp_buff, val_str));

    ret = ht_del(g_ht, key_str, strlen(key_str) + 1);
    YUNIT_ASSERT(0 == ret);

    len = sizeof(tmp_buff);
    memset(tmp_buff,0,len);
    ht_find(g_ht, key_str, strlen(key_str) + 1, tmp_buff, &len);
    YUNIT_ASSERT(0 != strcmp(tmp_buff, val_str));
}

static void test_ht_lockless(void)
{
    int key_int_1 = 1;
    int key_int_2 = 2;
    int key_int_3 = 3;

    char *val_str_1 = "1";
    char *val_str_2 = "2";
    char *val_str_3 = "3";

    int ret = 0;
    char tmp[5] = {0};
    int len = sizeof(tmp);

    ht_lock(g_ht);
    ret = ht_add_lockless(g_ht,&key_int_1,sizeof(int *),val_str_1,strlen(val_str_1)+1); 
    YUNIT_ASSERT(0 == ret);

    ret = ht_add_lockless(g_ht,&key_int_2,sizeof(int *),val_str_2,strlen(val_str_2)+1); 
    YUNIT_ASSERT(0 == ret);
    
    ret = ht_add_lockless(g_ht,&key_int_3,sizeof(int *),val_str_3,strlen(val_str_3)+1); 
    YUNIT_ASSERT(0 == ret);
    ht_unlock(g_ht);
    
    char *x = NULL;
    x = ht_find(g_ht,&key_int_1,sizeof(int *),tmp,&len);
    YUNIT_ASSERT(0 == strcmp(tmp,val_str_1));
    YUNIT_ASSERT(0 == strcmp(x,val_str_1));
}

static int init(void)
{
    g_ht = ht_init(g_ht_size);
    YUNIT_ASSERT(NULL != g_ht);

    return 0;
}

static int cleanup(void)
{
    int ret = 0;

    ret = ht_clear(g_ht);
    YUNIT_ASSERT(ret == 0);

    ret = ht_destroy(g_ht);
    YUNIT_ASSERT(ret == 0);

    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_hashtable_testcases[] = {
    { "ht_add", test_ht_add },
    { "ht_find", test_ht_find },
    { "ht_del", test_ht_del },
    { "ht_lockless", test_ht_lockless },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "hashtable", init, cleanup, setup, teardown, aos_hashtable_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_hashtable(void)
{    
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_hashtable);
