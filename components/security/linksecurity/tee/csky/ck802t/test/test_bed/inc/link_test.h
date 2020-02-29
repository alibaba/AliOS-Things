/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _LINK_TEST_H_
#define _LINK_TEST_H_

typedef struct _test_vec_t
{
    char *name;
    int (*test_func)(void);
} test_vec_t;

#define TABLE_ATTR(name) __attribute__((__section__(".table." #name ".data")))

#define TEST_ADD(func, name_str) \
    test_vec_t _##func##_ TABLE_ATTR(testtree) = { name_str, func }

#endif /* _LINK_TEST_H_ */
