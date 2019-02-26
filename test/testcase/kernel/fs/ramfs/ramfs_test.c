/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "aos/kernel.h"
#include "aos/vfs.h"

#include <yunit.h>
#include <yts.h>

extern int32_t ramfs_register(const char *mount_path);

static const char *g_string         = "ramfs test string.";
static const char *g_filepath       = "/RAMFS/test.txt";
static const char *g_dirpath        = "/RAMFS/testDir";
static const char *g_dirtest_1      = "/RAMFS/testDir/test_1.txt";
static const char *g_dirtest_2      = "/RAMFS/testDir/test_2.txt";
static const char *g_dirtest_3      = "/RAMFS/testDir/test_3.txt";
static const char *g_new_filepath   = "/RAMFS/testDir/newname.txt";

static void test_ramfs_case(void)
{
    int ret, fd;
    char readBuffer[32];
    aos_dir_t *dp;

    /* ramfs write test */
    fd = aos_open(g_filepath, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);

    if (fd > 0) {
        ret = aos_write(fd, g_string, strlen(g_string));
        YUNIT_ASSERT(ret > 0);
        ret = aos_sync(fd);
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* ramfs read test */
    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0) {
        ret = aos_read(fd, readBuffer, sizeof(readBuffer));
        YUNIT_ASSERT(ret > 0);

        ret = memcmp(readBuffer, g_string, strlen(g_string));
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* ramfs readdir test */
    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_2, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_3, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    dp = (aos_dir_t *)aos_opendir(g_dirpath);
    YUNIT_ASSERT(dp != NULL);

    if (dp) {
        aos_dirent_t *out_dirent;
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;

            printf("file name is %s\n", out_dirent->d_name);
        }
    }
    aos_closedir(dp);

    /* ramfs rename test */
    ret = aos_rename(g_filepath, g_new_filepath);
    YUNIT_ASSERT(ret == 0);

    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd < 0);
    if (fd >= 0)
        aos_close(fd);

    fd = aos_open(g_new_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    /* ramfs unlink test */
    ret = aos_unlink(g_new_filepath);
    YUNIT_ASSERT(ret == 0);

    fd = aos_open(g_new_filepath, O_RDONLY);
    YUNIT_ASSERT(fd < 0);
    if (fd > 0)
        aos_close(fd);
}

static int init(void)
{
    int ret = 0;

    ret = ramfs_register(NULL);
    YUNIT_ASSERT(ret == 0);
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

static yunit_test_case_t aos_ramfs_testcases[] = {
    { "ramfs_test", test_ramfs_case},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "ramfs", init, cleanup, setup, teardown, aos_ramfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_ramfs(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_ramfs);

