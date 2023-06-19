/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "yunit/yunit.h"
#include "fs_test.h"

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

void aos_fs_test(void)
{
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("fsapi", init, cleanup, setup, teardown);

	yunit_add_test_case(suite, "fs.open", test_fs_open);
    yunit_add_test_case(suite, "fs_write", test_fs_write);
    yunit_add_test_case(suite, "fs_read", test_fs_read);
    yunit_add_test_case(suite, "fs_stat", test_fs_stat);
    yunit_add_test_case(suite, "fs_statfs", test_fs_statfs);
    yunit_add_test_case(suite, "fs_link", test_fs_link);
    // yunit_add_test_case(suite, "fs_fcntl", test_fs_fcntl);
    //yunit_add_test_case(suite, "fs_pathconf", test_fs_pathconf);
    //yunit_add_test_case(suite, "fs_fpathconf", test_fs_fpathconf);
    //yunit_add_test_case(suite, "fs_utime", test_fs_utime);
    yunit_add_test_case(suite, "fs_close", test_fs_close);
    //yunit_add_test_case(suite, "fs_unlink", test_fs_unlink);
    yunit_add_test_case(suite, "fs_fd_leak", test_fs_fd_leak);
    yunit_add_test_case(suite, "fs_mkdir", test_fs_mkdir);
    yunit_add_test_case(suite, "fs_readdir", test_fs_readdir);
    //yunit_add_test_case(suite, "fs_chdir", test_fs_chdir);
    yunit_add_test_case(suite, "fs_seekdir", test_fs_seekdir);
    yunit_add_test_case(suite, "fs_rmdir", test_fs_rmdir);
 //   yunit_add_test_case(suite, "fs_mount", test_fs_mount);
}

AOS_TESTCASE(aos_fs_test);
