/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "fs_test.h"
#include "aos/vfs.h"
#include "aos/errno.h"

extern int test_file_write(void);
extern int test_file_close(void);
static int file;

static int test_mkdir(void)
{
    int res;
    int fd;

    TC_PRINT("\nmkdir tests:\n");

    /* Verify mkdir() */
    res = aos_mkdir(TEST_DIR);
    if (res) {
        TC_PRINT("Error creating dir[%d]\n", res);
        return res;
    }

    res = aos_open(TEST_DIR_FILE, O_CREAT | O_RDWR);
    if (res < 0) {
        TC_PRINT("Failed opening file [%d]\n", res);
        return res;
    }
    file = res;

    fd = aos_open(TEST_DIR_FILE1, O_CREAT | O_RDWR);
    if (fd < 0) {
        TC_PRINT("Failed opening file %s, ret: [%d]\n", TEST_DIR_FILE1, fd);
        return fd;
    }

    res = test_file_write();
    if (res) {
        return res;
    }

    res = aos_close(file);
    if (res) {
        TC_PRINT("Error closing file [%d]\n", res);
        return res;
    }

    res = aos_close(fd);
    if (res) {
        TC_PRINT("Error closing file %s,  ret: [%d]\n", TEST_DIR_FILE1, res);
        return res;
    }

    TC_PRINT("Created dir %s!\n", TEST_DIR);

    return TC_PASS;
}

static int test_lsdir(const char *path)
{
    aos_dir_t *dirp;
    aos_dirent_t *entry;
    char dirent1_name[256];
    char dirent2_name[256];
    long loc1;
    long loc2;

    TC_PRINT("\nreaddir test:\n");

    /* Verify opendir() */
    dirp = aos_opendir(path);
    if (dirp == NULL) {
        TC_PRINT("Error opening dir %s\n", path);
        return -EIO;
    }

    TC_PRINT("\nListing dir %s:\n", path);
    /* Verify readdir() */
    entry = aos_readdir(dirp);
    strcpy(dirent1_name, entry->d_name);
    while (entry != NULL) {
        /* Verify telldir() */
        long location = aos_telldir(dirp);
        TC_PRINT("[FILE] name: %s, location:%ld\n", entry->d_name, location);
        entry = aos_readdir(dirp);
    }

    /* Verify rewinddir() */
    aos_rewinddir(dirp);
    entry = aos_readdir(dirp);
    strcpy(dirent2_name, entry->d_name);

    if (strcmp(dirent1_name, dirent2_name)) {
        TC_PRINT("rewinddir failed, dirent1 name:%s != dirent2 name:%s\n", dirent1_name, dirent2_name);
        return TC_FAIL;
    }

    /* Verify closedir() */
    aos_closedir(dirp);

    return 0;
}

static int test_seekdir(const char *path)
{
    aos_dir_t *dirp;
    aos_dirent_t *entry;
    long loc1;
    long loc2;

    TC_PRINT("\nseekdir test:\n");

    /* Verify opendir() */
    dirp = aos_opendir(path);
    if (dirp == NULL) {
        TC_PRINT("Error opening dir %s\n", path);
        return -EIO;
    }

    TC_PRINT("\nListing dir %s:\n", path);
    /* Verify telldir() */
    loc1 = aos_telldir(dirp);
    if (loc1 < 0) {
        TC_PRINT("telldir failed, ret:%ld\n", loc1);
        return TC_FAIL;
    }
    /* Verify readdir() */
    do {
        entry = aos_readdir(dirp);
        long location = aos_telldir(dirp);
        if (entry)
            TC_PRINT("[FILE] name: %s, location:%ld\n", entry->d_name, location);
    } while (entry != NULL);

    /* Seek to the beginning of the directory */
    aos_seekdir(dirp, loc1);
    loc2 = aos_telldir(dirp);
    if (loc2 < 0) {
        TC_PRINT("telldir failed, ret:%ld\n", loc2);
        return TC_FAIL;
    }
    if (loc1 != loc2) {
        TC_PRINT("seekdir failed, loc1:%ld != loc2:%ld\n", loc1, loc2);
        return TC_FAIL;
    }

    /* Verify closedir() */
    aos_closedir(dirp);

    return 0;
}

#if 0
static int test_chdir(void)
{
    int res;
    char buf[256] = {0};
    char *pres = NULL;

    /*Verify chdir*/
    res = chdir(ROOTFS_MNTP);
    if (res < 0) {
        TC_PRINT("Error change dir %s\n", ROOTFS_MNTP);
        return TC_FAIL;
    }

    /*Verify getcwd*/
    pres = getcwd(buf, sizeof(buf) - 1);
    if (pres == NULL) {
        TC_PRINT("Error getcwd.\n");
        return TC_FAIL;
    }

    res = strcmp(buf, ROOTFS_MNTP);
    if (res != 0) {
        TC_PRINT("change dir failed.\n");
        return TC_FAIL;
    }

    return TC_PASS;
}
#endif

static int test_rmdir(void)
{
    int res;
    int retn = TC_PASS;

    TC_PRINT("\nrmdir tests:\n");

    res = aos_remove(TEST_DIR_FILE);
    if (res) {
        TC_PRINT("Error deleting file [%d]\n", res);
        retn = TC_FAIL;
    }

    res = aos_remove(TEST_DIR_FILE1);
    if (res) {
        TC_PRINT("Error deleting file [%d]\n", res);
        retn = TC_FAIL;
    }

    /* Verify rmdir() */
    res = aos_rmdir(TEST_DIR);
    if (res) {
        TC_PRINT("Error rmdir %s [%d]\n", TEST_DIR, res);
        retn = TC_FAIL;
    }

    return retn;

}

#if 0
/**
 * 测试标题：文件操作API(chdir/getcwd)功能测试
 * 输入条件：/data/目录以挂载文件系统，创建目录TEST_DIR
 * 测试步骤：
 * 1. 切换目录到/data/
 * 2. getcwd获取当前目录
 * 3. 比较步骤2的结果是/data/
 * 预期结果：所有调用API返回值正常， 且步骤3检查结果相等
 */
void test_fs_chdir(void)
{
    YUNIT_ASSERT(test_chdir() == TC_PASS);
}
#endif

/**
 * 测试标题：文件操作API(opendir/telldir/readdir/rewinddir/closedir)功能测试
 * 输入条件：/data/目录以挂载文件系统，创建目录TEST_DIR
 * 测试步骤：
 * 1. opendir打开TEST_DIR目录
 * 2. telldir获取位置1
 * 3. 循环读目录数据直到结束
 * 4. seekdir回到目录数据的开始处
 * 5. telldir获取位置2
 * 6. 检查位置1与位置2是否相等
 * 7. 关闭TEST_DIR目录
 * 预期结果：所有调用API返回值正常， 且步骤6结果是相等
 */
void test_fs_seekdir(void)
{
    YUNIT_ASSERT(test_seekdir(TEST_DIR) == TC_PASS);
}

/**
 * 测试标题：文件操作API(mkdir)功能测试
 * 输入条件：/data/目录以挂载文件系统
 * 测试步骤：
 * 1. mkdir创建目录TEST_DIR
 * 2. 创建文件打开并写入内容，然后关闭文件
 * 预期结果：所有调用API返回值正常
 */
void test_fs_mkdir(void)
{
    YUNIT_ASSERT(test_mkdir() == TC_PASS);
}

/**
 * 测试标题：文件操作API(opendir/telldir/readdir/rewinddir/closedir)功能测试
 * 输入条件：/data/目录以挂载文件系统，创建目录TEST_DIR
 * 测试步骤：
 * 1. opendir打开TEST_DIR目录
 * 2. telldir获取位置1
 * 3. 循环读目录数据直到结束
 * 4. rewinddir回到目录数据的开始处
 * 5. telldir获取位置2
 * 6. 检查位置1与位置2是否相等
 * 7. 关闭TEST_DIR目录
 * 预期结果：所有调用API返回值正常， 且步骤6结果是相等
 */
void test_fs_readdir(void)
{
    YUNIT_ASSERT(test_lsdir(TEST_DIR) == TC_PASS);
}

/**
 * 测试标题：文件操作API(rmdir)功能测试
 * 输入条件：/data/目录以挂载文件系统，创建目录TEST_DIR
 * 测试步骤：
 * 1. unlink删除TEST_DIR目录下的文件
 * 2. rmdir删除目录TEST_DIR
 * 预期结果：所有调用API返回值正常
 */
void test_fs_rmdir(void)
{
    YUNIT_ASSERT(test_rmdir() == TC_PASS);
}
