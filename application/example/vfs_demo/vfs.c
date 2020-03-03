/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/vfs.h"

extern int32_t ramfs_register(const char *mount_path);
extern int32_t ramfs_unregister(const char *mount_path);

static const char *g_string         = "ramfs test string.";
static const char *g_filepath       = "/RAMFS/test.txt";
static const char *g_dirpath        = "/RAMFS/testDir";
static const char *g_dirtest_1      = "/RAMFS/testDir/test_1.txt";
static const char *g_dirtest_2      = "/RAMFS/testDir/test_2.txt";
static const char *g_dirtest_3      = "/RAMFS/testDir/test_3.txt";
static const char *g_new_filepath   = "/RAMFS/testDir/newname.txt";

#define VFS_ASSERT(cond) \
    if (!(cond)) { \
        printf("%s:%d asserted, condition %s\r\n", __func__, __LINE__, #cond); \
        while(1); \
    }

/**
 * ramfs has been registered when init the test case.
 * here we need to unregister ramfs firstly, and before
 * leaving the testcase, register ramfs again.
 */
static void test_vfs_mount(void)
{
    int i;
    int ret;
    const int cnt = 100;

    printf("  VFS register/unregister test start\r\n");

    ret = ramfs_unregister(NULL);
    VFS_ASSERT(ret == 0);

    for (i = 0; i < cnt; i++) {
        ret = ramfs_register(NULL);
        VFS_ASSERT(ret == 0);

        ret = ramfs_unregister(NULL);
        VFS_ASSERT(ret == 0);
    }

    ret = ramfs_register(NULL);
    VFS_ASSERT(ret == 0);

    printf("  VFS register/unregister test OK\r\n");
}

static void test_vfs_rw(void)
{
    int fd;
    int *buf;
    int buf_len;
    int write_len, read_len;
    int temp_size;
    int i;
    int ret;
    const int file_size = 328 * sizeof(int);

    printf("  VFS test_ramfs_rw test start\r\n");

    fd = aos_open(g_filepath, O_WRONLY | O_CREAT | O_TRUNC);
    VFS_ASSERT(fd > 0);

    buf_len = 32*sizeof(int);
    buf = malloc(buf_len);
    VFS_ASSERT(buf != NULL);
    memset(buf, 0, buf_len);
    for (i = 0; i < buf_len/sizeof(int); i++) {
        buf[i] = i;
    }

    for (write_len = 0; write_len < file_size; ) {
        int remain = file_size - write_len;
        temp_size = (remain > buf_len)? buf_len: remain;
        ret = aos_write(fd, buf, temp_size);
        VFS_ASSERT(ret  == temp_size);
        write_len += temp_size;
    }

    ret = aos_sync(fd);
    VFS_ASSERT(ret == 0);
    ret = aos_close(fd);
    VFS_ASSERT(ret == 0);

    fd = aos_open(g_filepath, O_RDONLY);
    VFS_ASSERT(fd > 0);

    /* read the file size */
    int file_end = aos_lseek(fd, 0, SEEK_END);
    VFS_ASSERT(file_end == file_size - 1);

    /* set the file read/write anchor at the beginning of the file */
    ret = aos_lseek(fd, 0, SEEK_SET);
    VFS_ASSERT(ret == 0);

    int ctx_err = 0;
    for (read_len = 0; read_len < file_size; ) {
        int remain = file_size - read_len;
        temp_size = (remain > buf_len)? buf_len: remain;
        ret = aos_read(fd, buf, temp_size);
        VFS_ASSERT(ret  == temp_size);
        for (i = 0; i < temp_size/sizeof(int); i++) {
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

    VFS_ASSERT(ctx_err == 0);

    ret = aos_close(fd);
    VFS_ASSERT(ret == 0);

    /* remove the file */
    ret = aos_unlink(g_filepath);
    VFS_ASSERT(ret == 0);

    printf("  VFS test_ramfs_rw test OK\r\n");
}

static void test_vfs_dir(void)
{
    int ret;
    int fd;

    printf("  VFS test_ramfs_dir test start\r\n");

    ret = aos_mkdir(g_dirpath);
    VFS_ASSERT(ret == 0);

    /* the dir already exists, returan fail */
    ret = aos_mkdir(g_dirpath);
    VFS_ASSERT(ret != 0);

    /* creat file under the dir */
    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT);
    VFS_ASSERT(fd > 0);

    ret = aos_write(fd, g_string, strlen(g_string));
    VFS_ASSERT(ret > 0);

    ret = aos_close(fd);
    VFS_ASSERT(ret == 0);

    /* rmdir should fail as the dir is not empty */
    ret = aos_rmdir(g_dirpath);
    VFS_ASSERT(ret != 0);

    /* remove the file in the dir and then rmdir */
    ret = aos_unlink(g_dirtest_1);
    VFS_ASSERT(ret == 0);

    ret = aos_rmdir(g_dirpath);
    VFS_ASSERT(ret == 0);

    printf("  VFS test_ramfs_dir test OK\r\n");
}

static void test_vfs_integrated(void)
{
    int ret, fd;
    char readBuffer[32];
    aos_dir_t *dp;

    printf("  VFS test_ramfs_integrated test start\r\n");

    /* write test */
    fd = aos_open(g_filepath, O_RDWR | O_CREAT | O_TRUNC);
    VFS_ASSERT(fd > 0);

    if (fd > 0) {
        ret = aos_write(fd, g_string, strlen(g_string));
        VFS_ASSERT(ret > 0);
        ret = aos_sync(fd);
        VFS_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* read test */
    fd = aos_open(g_filepath, O_RDONLY);
    VFS_ASSERT(fd > 0);
    if (fd > 0) {
        ret = aos_read(fd, readBuffer, sizeof(readBuffer));
        VFS_ASSERT(ret > 0);

        ret = memcmp(readBuffer, g_string, strlen(g_string));
        VFS_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* mkdir test */
    ret = aos_mkdir(g_dirpath);
    VFS_ASSERT(ret == 0);

    /* readdir test */
    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT | O_TRUNC);
    VFS_ASSERT(fd > 0);
    if (fd > 0) {
        aos_close(fd);
    }

    fd = aos_open(g_dirtest_2, O_RDWR | O_CREAT | O_TRUNC);
    VFS_ASSERT(fd > 0);
    if (fd > 0) {
        aos_close(fd);
    }

    fd = aos_open(g_dirtest_3, O_RDWR | O_CREAT | O_TRUNC);
    VFS_ASSERT(fd > 0);
    if (fd > 0) {
        aos_close(fd);
    }

    dp = (aos_dir_t *)aos_opendir(g_dirpath);
    VFS_ASSERT(dp != NULL);

    if (dp) {
        aos_dirent_t *out_dirent;
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;

            printf("    file name is %s\n", out_dirent->d_name);
        }
    }
    aos_closedir(dp);

    /* rename test */
    ret = aos_rename(g_filepath, g_new_filepath);
    VFS_ASSERT(ret == 0);

    fd = aos_open(g_filepath, O_RDONLY);
    VFS_ASSERT(fd < 0);
    if (fd >= 0) {
        aos_close(fd);
    }

    fd = aos_open(g_new_filepath, O_RDONLY);
    VFS_ASSERT(fd > 0);
    if (fd > 0) {
        aos_close(fd);
    }

    /* unlink test */
    ret = aos_unlink(g_new_filepath);
    VFS_ASSERT(ret == 0);

    fd = aos_open(g_new_filepath, O_RDONLY);
    VFS_ASSERT(fd < 0);
    if (fd > 0) {
        aos_close(fd);
    }

    printf("  VFS test_ramfs_integrated test OK\r\n");
}

static int init(void)
{
    int ret = 0;

    ret = ramfs_register(NULL);
    VFS_ASSERT(ret == 0);

    if (ret == 0) {
        printf("  vfs_app init OK\r\n");
    } else {
        printf("  vfs_app init fail, ret %d\r\n", ret);
    }

    return ret;
}

static int cleanup(void)
{
    int ret;

    ret = ramfs_unregister(NULL);
    VFS_ASSERT(ret == 0);

    if (ret == 0) {
        printf("  vfs_app cleanup OK\r\n");
    } else {
        printf("  vfs_app cleanup fail, ret %d\r\n", ret);
    }

    return 0;
}

int application_start(int argc, char *argv[])
{
    printf("vfs app start\r\n");

    init();

    test_vfs_mount();
    test_vfs_rw();
    test_vfs_dir();
    test_vfs_integrated();

    cleanup();

    printf("vfs app end\r\n");

    while(1) {
        aos_msleep(1000);
    }
}

