/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include <sqlite.h>

void sqlite_demo(void *arg)
{
    int count = 0;
    const char *testdb = "/data/test.db";

    (void)arg;
    printf("sqlite demo entry here!\r\n");
    printf("***create table test start***\r\n");
    sqlite_test_createtab(testdb);
    printf("***create table test end***\r\n");
    printf("***delete table test start***\r\n");
    sqlite_test_deletetab(testdb);
    printf("***delete table test end***\r\n");
    sqlite_test_tabview(testdb, "t1");

    while(1) {
        printf("Hello SQLite demo! count %d \r\n", count++);
        aos_msleep(1000);
    };
}

