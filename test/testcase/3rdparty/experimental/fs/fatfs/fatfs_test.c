/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "aos/kernel.h"
#include "fs/vfs.h"

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include "fs/fatfs.h"

static const char *g_string         = "Fatfs test string.";
static const char *g_filepath       = "/sdcard/test.txt";
static const char *g_dirpath        = "/sdcard/testDir";
static const char *g_dirtest_1      = "/sdcard/testDir/test_1.txt";
static const char *g_dirtest_2      = "/sdcard/testDir/test_2.txt";
static const char *g_dirtest_3      = "/sdcard/testDir/test_3.txt";
static const char *g_new_filepath   = "/sdcard/testDir/newname.txt";

static void test_fatfs_case(void)
{
    int ret, fd;
    char readBuffer[32];

    /* Fatfs write test */
    fd = aos_open(g_filepath, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);

    if (fd > 0) {
        ret = aos_write(fd, g_string, strlen(g_string));
        YUNIT_ASSERT(ret > 0);
        ret = aos_sync(fd);
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* Fatfs read test */
    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0) {
        ret = aos_read(fd, readBuffer, sizeof(readBuffer));
        YUNIT_ASSERT(ret > 0);

        ret = memcmp(readBuffer, g_string, strlen(g_string));
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* Fatfs mkdir test */
    aos_dir_t *dp = (aos_dir_t *)aos_opendir(g_dirpath);
    if (!dp) {
        ret = aos_mkdir(g_dirpath);
        YUNIT_ASSERT(ret == 0);
    } else {
        ret = aos_closedir(dp);
        YUNIT_ASSERT(ret == 0);
    }

    /* Fatfs readdir test */
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

    /* Fatfs rename test */
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

    /* Fatfs unlink test */
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

    ret = fatfs_register();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static int cleanup(void)
{
    int ret = fatfs_unregister();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_fatfs_testcases[] = {
    { "fatfs_test", test_fatfs_case},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "fatfs", init, cleanup, setup, teardown, aos_fatfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_fatfs(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_fatfs);

