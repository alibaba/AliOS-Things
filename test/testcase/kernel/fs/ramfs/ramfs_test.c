/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "aos/kernel.h"
#include "aos/vfs.h"

#include "yunit/yunit.h"
#include "yunit/yts.h"

extern int32_t ramfs_register(const char *mount_path);
extern int32_t ramfs_unregister(const char *mount_path);


static const char *g_string         = "ramfs test string.";
static const char *g_filepath       = "/RAMFS/test.txt";
static const char *g_dirpath        = "/RAMFS/testDir";
static const char *g_dirtest_1      = "/RAMFS/testDir/test_1.txt";
static const char *g_dirtest_2      = "/RAMFS/testDir/test_2.txt";
static const char *g_dirtest_3      = "/RAMFS/testDir/test_3.txt";
static const char *g_new_filepath   = "/RAMFS/testDir/newname.txt";

/**
 * ramfs has been registered when init the test case.
 * here we need to unregister ramfs firstly, and before
 * leaving the testcase, register ramfs again.
 */
static void test_ramfs_mount(void)
{
    int ret;
    const int cnt = 100;

    printf("ramfs register/unregister test start\r\n");

    ret = ramfs_unregister(NULL);
    YUNIT_ASSERT(ret == 0);

    for (int i = 0; i < cnt; i++) {
        ret = ramfs_register(NULL);
        YUNIT_ASSERT(ret == 0);

        ret = ramfs_unregister(NULL);
        YUNIT_ASSERT(ret == 0);
    }

    ret = ramfs_register(NULL);
    YUNIT_ASSERT(ret == 0);

    printf("ramfs register/unregister test OK\r\n");
}

static void test_ramfs_rw(void)
{
    int fd;
    int *buf;
    int buf_len;
    int write_len, read_len;
    int temp_size;
    int ret;
    const int file_size = 328 * sizeof(int);

    printf("ramfs test_ramfs_rw test start\r\n");

    fd = aos_open(g_filepath, O_WRONLY | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);

    buf_len = 32*sizeof(int);
    buf = malloc(buf_len);
    YUNIT_ASSERT(buf != NULL);
    memset(buf, 0, buf_len);
    for (int i = 0; i < buf_len/sizeof(int); i++) {
        buf[i] = i;
    }

    for (write_len = 0; write_len < file_size; ) {
        int remain = file_size - write_len;
        temp_size = (remain > buf_len)? buf_len: remain;
        ret = aos_write(fd, buf, temp_size);
        YUNIT_ASSERT(ret  == temp_size);
        write_len += temp_size;
    }

    ret = aos_sync(fd);
    YUNIT_ASSERT(ret == 0);
    ret = aos_close(fd);
    YUNIT_ASSERT(ret == 0);

    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);

    int file_end = aos_lseek(fd, 0, SEEK_END);
    YUNIT_ASSERT(file_end == file_size - 1);

    ret = aos_lseek(fd, 0, SEEK_SET);
    YUNIT_ASSERT(ret == 0);

    int ctx_err = 0;
    for (read_len = 0; read_len < file_size; ) {
        int remain = file_size - read_len;
        temp_size = (remain > buf_len)? buf_len: remain;
        ret = aos_read(fd, buf, temp_size);
        YUNIT_ASSERT(ret  == temp_size);
        for (int i = 0; i < temp_size/sizeof(int); i++) {
            if (buf[i] != i) {
                ctx_err = 1;
                break;
            }
        }
        if (ctx_err != 0) {
            break;
        }
        read_len += temp_size;
    }

    YUNIT_ASSERT(ctx_err == 0);

    ret = aos_close(fd);
    YUNIT_ASSERT(ret == 0);

    ret = aos_unlink(g_filepath);
    YUNIT_ASSERT(ret == 0);

    printf("ramfs test_ramfs_rw test OK\r\n");
}

static void test_ramfs_dir(void)
{
    int ret;
    int fd;

    ret = aos_mkdir(g_dirpath);
    YUNIT_ASSERT(ret == 0);

    /* the dir already exists, returan fail */
    ret = aos_mkdir(g_dirpath);
    YUNIT_ASSERT(ret != 0);

    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT);
    YUNIT_ASSERT(fd > 0);

    ret = aos_write(fd, g_string, strlen(g_string));
    YUNIT_ASSERT(ret > 0);

    ret = aos_close(fd);
    YUNIT_ASSERT(ret == 0);

    ret = aos_rmdir(g_dirpath);
    YUNIT_ASSERT(ret != 0);

    ret = aos_unlink(g_dirtest_1);
    YUNIT_ASSERT(ret == 0);

    ret = aos_rmdir(g_dirpath);
    YUNIT_ASSERT(ret == 0);

}

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

    /* ramfs mkdir test */
    ret = aos_mkdir(g_dirpath);
    YUNIT_ASSERT(ret == 0);

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
    int ret;

    ret = ramfs_unregister(NULL);
    YUNIT_ASSERT(ret == 0);

    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_ramfs_testcases[] = {
    {"mount_test", test_ramfs_mount},
    {"read_write_test", test_ramfs_rw},
    {"dir_operation", test_ramfs_dir},
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

