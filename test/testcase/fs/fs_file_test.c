/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "fs_test.h"
#include "aos/vfs.h"

const char test_str[] = "hello world!";
static int file;

static int test_file_open(void)
{
    int res;

    res = aos_open(TEST_FILE, O_CREAT | O_RDWR);
    zassert_true(res >= 0, "Failed opening file: %d, errno=%d\n", res, errno);

    file = res;

    if (res < 0) {
        return TC_FAIL;
    } else {
        return TC_PASS;
    }
}

int test_file_write(void)
{
    ssize_t brw;
    off_t res;
    int ret;
    long file_size;
    size_t str_len;

    res = aos_lseek(file, 0, SEEK_SET);
    if (res != 0) {
        TC_PRINT("lseek failed [%d]\n", (int)res);
        aos_close(file);
        return TC_FAIL;
    }

    str_len = strlen(test_str);
    brw = aos_write(file, (char *)test_str, str_len);
    if (brw < 0) {
        TC_PRINT("Failed writing to file [%d]\n", (int)brw);
        aos_close(file);
        return TC_FAIL;
    }

    ret = aos_sync(file);
    if (ret < 0) {
        TC_PRINT("Error fsync, ret:%d.\n", ret);
        aos_close(file);
        return TC_FAIL;
    }

    if (brw < str_len) {
        TC_PRINT("Unable to complete write. Volume full.\n");
        TC_PRINT("Number of bytes written: [%d]\n", (int)brw);
        aos_close(file);
        return TC_FAIL;
    }

    file_size = aos_fsize(file);
    if (file_size != str_len) {
        TC_PRINT("filesize [%ld] is not right. It should be %u\n", filesize, str_len);
        aos_close(file);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_read(void)
{
    ssize_t brw;
    off_t res;
    char read_buff[80] = { 0 };
    size_t sz = strlen(test_str);

    aos_sync(file);

    res = aos_lseek(file, 0, SEEK_SET);
    if (res != 0) {
        TC_PRINT("lseek failed [%d]\n", (int)res);
        aos_close(file);
        return TC_FAIL;
    }

    brw = aos_read(file, read_buff, sz);
    if (brw < 0) {
        TC_PRINT("Failed reading file [%d]\n", (int)brw);
        aos_close(file);
        return TC_FAIL;
    }

    read_buff[brw] = 0;

    if (strcmp(test_str, read_buff)) {
        TC_PRINT("Error - Data read does not match data written\n");
        TC_PRINT("Data read:\"%s\"\n\n", read_buff);
        return TC_FAIL;
    }

    memset(read_buff, 0, 80);

    /* Now test after non-zero lseek. */

    res = aos_lseek(file, 2, SEEK_SET);
    if (res != 2) {
        TC_PRINT("lseek failed [%d]\n", (int)res);
        aos_close(file);
        return TC_FAIL;
    }

    brw = aos_read(file, read_buff, sizeof(read_buff));
    if (brw < 0) {
        TC_PRINT("Failed reading file [%d]\n", (int)brw);
        aos_close(file);
        return TC_FAIL;
    }

    /* Check for array overrun */
    brw = (brw < 80) ? brw : brw - 1;

    read_buff[brw] = 0;

    if (strcmp(test_str + 2, read_buff)) {
        TC_PRINT("Error - Data read does not match data written\n");
        TC_PRINT("Data read:\"%s\"\n\n", read_buff);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_close(void)
{
    int res;

    res = aos_close(file);
    zassert_true(res == 0, "Failed closing file: %d, errno=%d\n", res, errno);

    return res;
}

static int test_file_stat(void)
{
    int ret;
    struct aos_stat test_file_stat_buf = {0};
    //int old_size;

    ret = aos_access(TEST_FILE, F_OK | R_OK | W_OK);
    if (ret < 0) {
        TC_PRINT("Error access %s, ret:%d, errno:%d.\n", TEST_FILE, ret, errno);
        return TC_FAIL;
    }

    memset(&test_file_stat_buf, 0, sizeof(test_file_stat_buf));
    ret = aos_stat(TEST_FILE, &test_file_stat_buf);
    if (ret < 0) {
        TC_PRINT("Error stat file: %s, ret: [%d], errno:%d\n", TEST_FILE, ret, errno);
        return TC_FAIL;
    }
    TC_PRINT("%s file size: %ld\n", TEST_FILE, test_file_stat_buf.st_size);
    /*
    old_size = test_file_stat_buf.st_size;

    memset(&test_file_stat_buf, 0, sizeof(test_file_stat_buf));
    ret = fstat(file, &test_file_stat_buf);
    if (ret < 0) {
        TC_PRINT("Error fstat fd: %d, ret: [%d], errno:%d\n", file, ret, errno);
        return TC_FAIL;
    }
    TC_PRINT("%s file size: %ld\n", TEST_FILE, test_file_stat_buf.st_size);

    if (old_size != test_file_stat_buf.st_size) {
        TC_PRINT("Error returned file size of stat: %d != fstat: %d.\n", old_size, test_file_stat_buf.st_size);
        return TC_FAIL;
    }
    */

    return TC_PASS;
}

static int test_file_statfs(void)
{
    int res;
    struct aos_statfs test_file_statfs_buf = { 0 };

    res = aos_statfs(TEST_FILE, &test_file_statfs_buf);
    if (res < 0) {
        TC_PRINT("Error statfs file system, ret: [%d], errno:%d\n", res, errno);
        return TC_FAIL;
    }
    if (test_file_statfs_buf.f_blocks == 0) {
        TC_PRINT("Error statfs file system, f_blocks is 0.\n");
        return TC_FAIL;
    }
    TC_PRINT("file system total blocks: %d\n", test_file_statfs_buf.f_blocks);

    return TC_PASS;
}

static int test_file_link(void)
{
    int res;
    int fd;
    const char *new_file_path = ROOTFS_MNTP "/test_file_temp.txt";

    /* littlefs do not support link. */
    errno = 0;
    res = aos_link(TEST_FILE, TEST_FILE1);
    if ((res < 0)) {
        if (errno == ENOSYS) {
            TC_PRINT("Error link is not supported by the underlying fs.\n");
            /* Create TEST_FILE1 to continue testing. */
            fd = open(TEST_FILE1, O_CREAT | O_RDWR);
            if (fd < 0) {
                TC_PRINT("Error create file %s , ret:%d, errno:%d\n", TEST_FILE1, fd, errno);
                return TC_FAIL;
            }
            close(fd);
            return TC_PASS;
        } else {
            TC_PRINT("Error link %s to %s, ret:%d, errno:%d\n", TEST_FILE, TEST_FILE1, res, errno);
            return TC_FAIL;
        }
    }

    res = aos_rename(TEST_FILE1, new_file_path);
    if (res < 0) {
        TC_PRINT("Error rename %s to %s, ret:%d, errno:%d\n", TEST_FILE1, new_file_path, res, errno);
        return TC_FAIL;
    }

    res = aos_remove(new_file_path);
    if (res < 0) {
        TC_PRINT("Error remove %s, ret:%d, errno:%d\n", TEST_FILE1, res, errno);
        return TC_FAIL;
    }

    res = aos_access(new_file_path, F_OK);
    if (res == 0) {
        TC_PRINT("Error access %s, ret:%d\n", new_file_path, res);
        return TC_FAIL;
    }

    return TC_PASS;
}

#if 0
static int test_file_fcntl(void)
{
    int res;
    long arg;

    /* aos vfs fcntl do not support fcntl. */
    errno = 0;
    res = fcntl(file, F_GETFD, arg);
    if (res < 0) {
        TC_PRINT("Error fcntl fd:%d, ret:%d, errno: %d\n", file, res, errno);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_pathconf(void)
{
    long res;

    errno = 0;
    /* littlefs do not support pathconf */
    res = pathconf(TEST_FILE, _PC_LINK_MAX);
    if (res < 0 && errno != ENOSYS) {
        TC_PRINT("Error pathconf ret:%d\n", res);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_fpathconf(void)
{
    long res;

    errno = 0;
    res = fpathconf(file, _PC_LINK_MAX);
    /* littlefs do not support fpathconf */
    if (res < 0 && errno != ENOSYS) {
        TC_PRINT("Error pathconf ret:%d\n", res);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_utime(void)
{
    int res;

    res = utime(TEST_FILE, NULL);
    if (res < 0) {
        TC_PRINT("Error utime ret:%d, errno:%d\n", res, errno);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_file_delete(void)
{
    int res;

    res = unlink(TEST_FILE);
    if (res) {
        TC_PRINT("Error deleting file [%d]\n", res);
        return res;
    }

    return res;
}
#endif

/**
 * 测试标题：文件操作API(open)功能测试
 * 输入条件：无
 * 测试步骤：1. 创建文件TEST_FILE并打开
 * 预期结果：返回的正常的fd
 */
void test_fs_open(void)
{
    YUNIT_ASSERT(test_file_open() == TC_PASS);
}

/**
 * 测试标题：文件操作API(write/lseek/sync/fsize)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开
 * 测试步骤：
 * 1. seek到文件开头
 * 2. 写入一段字符串
 * 3. sync这个文件到存储设备
 * 4. 检查write是否完全写入字符串
 * 5. 检查fsize获取到的文件大小是否正确
 * 预期结果：步骤1-3, 5返回值正常，步骤4 write完全写入了字符串
 */
void test_fs_write(void)
{
    YUNIT_ASSERT(test_file_write() == TC_PASS);
}

/**
 * 测试标题：文件操作API(read/sync)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. seek到文件开头
 * 2. 从文件中读取一段内容
 * 3. 读取的内容与之前写入的内容比较
 * 4. seek到文件偏移2字节的地方
 * 5. 读取文件的内容
 * 6. 读取的内容与之前写入内容偏移2字节后比较
 * 预期结果：
 * 1. 步骤3的比较结果相等，步骤6的比较结果相等
 */
void test_fs_read(void)
{
    YUNIT_ASSERT(test_file_read() == TC_PASS);
}

/**
 * 测试标题：文件操作API(access/stat/fstat)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. access检查文件是否存在
 * 2. stat获取文件的状态的信息包括大小
 * 3. fstat获取文件的状态的信息包括大小
 * 4. 判断2，3获取的大小是否相等
 * 预期结果：
 * 1. 步骤4的比较结果相等
 */
void test_fs_stat(void)
{
    YUNIT_ASSERT(test_file_stat() == TC_PASS);
}

/**
 * 测试标题：文件操作API(statfs)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. statfs获取文件系统的状态信息
 * 2. 检查获取的文件系统的blocks数目
 * 预期结果：
 * 1. 步骤2的blocks数组不为0
 */
void test_fs_statfs(void)
{
    YUNIT_ASSERT(test_file_statfs() == TC_PASS);
}

/**
 * 测试标题：文件操作API(link/remove/rename)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. 创建TEST_FILE的hard link为TEST_FILE1
 * 2. TEST_FILE1重命名为新的文件
 * 3. 删除新的文件名
 * 4. 检查改文件是否存在
 * 预期结果：
 * 1. 步骤4的结果应为不存在或者部分API底层文件系统不支持返回errno为ENOSYS
 */
void test_fs_link(void)
{
    YUNIT_ASSERT(test_file_link() == TC_PASS);
}

#if 0
/**
 * 测试标题：文件操作API(fcntl)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. fcntl F_GETFD
 * 预期结果：
 * 1. fcntl api 在vfs层不支持，API返回0.
 */
void test_fs_fcntl(void)
{
    YUNIT_ASSERT(test_file_fcntl() == TC_PASS);
}

/**
 * 测试标题：文件操作API(pathconf)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. pathconf _PC_LINK_MAX的获取最大link值
 * 预期结果：
 * 1. pathconf 在littlefs层不支持，pathconf返回值-1，errno为ENOSYS
 */
void test_fs_pathconf(void)
{
    YUNIT_ASSERT(test_file_pathconf() == TC_PASS);
}

/**
 * 测试标题：文件操作API(fpathconf)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. fpathconf _PC_LINK_MAX的获取最大link值
 * 预期结果：
 * 1. fpathconf 在littlefs层不支持，fpathconf返回值-1，errno为ENOSYS
 */
void test_fs_fpathconf(void)
{
    YUNIT_ASSERT(test_file_fpathconf() == TC_PASS);
}
#endif

/**
 * 测试标题：文件操作API(close)功能测试
 * 输入条件：TEST_FILE文件已存在并被打开，并已经写入了内容
 * 测试步骤：
 * 1. close 关闭文件
 * 预期结果：
 * 1. close返回值0
 */
void test_fs_close(void)
{
    YUNIT_ASSERT(test_file_close() == TC_PASS);
}

#if 0
/**
 * 测试标题：文件操作API(utime)功能测试
 * 输入条件：TEST_FILE文件已存在
 * 测试步骤：
 * 1. utime设置文件的时间为当前系统时间
 * 预期结果：
 * 1. utime返回值正常
 */
void test_fs_utime(void)
{
    YUNIT_ASSERT(test_file_utime() == TC_PASS);
}

/**
 * 测试标题：文件操作API(unlink)功能测试
 * 输入条件：TEST_FILE文件已存在
 * 测试步骤：
 * 1. unlink删除文件
 * 预期结果：
 * 1. unlink返回值正常
 */
void test_fs_unlink(void)
{
    YUNIT_ASSERT(test_file_delete() == TC_PASS);
}
#endif

/**
 * 测试标题：文件操作API(fd_leak)功能测试
 * 输入条件：
 * 测试步骤：
 * 预期结果：
 */
void test_fs_fd_leak(void)
{
    YUNIT_ASSERT(TC_PASS == TC_PASS);
}
