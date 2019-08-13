/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "aos/kernel.h"

#include "yunit/yunit.h"

#include "fs/vfs_types.h"
#include "fs/vfs_api.h"

#define VFS_FUNC_ATTACH 10001

static int g_vfs_func_entry = 0;

static vfs_dir_t g_vfs_test_dir;

static int32_t vfs_test_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    return 0;
}

static int32_t vfs_test_close(vfs_file_t *fp)
{
    return 0;
}

static int32_t vfs_test_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    return VFS_FUNC_ATTACH;
}

static uint32_t vfs_test_lseek(vfs_file_t *fp, uint32_t off, int32_t whence)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_sync(vfs_file_t *fp)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_fstat(vfs_file_t *fp, vfs_stat_t *st)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_link(vfs_file_t *fp, const char *path1, const char *path2)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_unlink(vfs_file_t *fp, const char *path)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_remove(vfs_file_t *fp, const char *path)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    return VFS_FUNC_ATTACH;
}

static vfs_dir_t *vfs_test_opendir(vfs_file_t *fp, const char *path)
{
    g_vfs_func_entry = 1;
    return &g_vfs_test_dir;
}

static vfs_dirent_t *vfs_test_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    g_vfs_func_entry = 1;
    return NULL;
}

static int32_t vfs_test_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_mkdir(vfs_file_t *fp, const char *path)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_rmdir(vfs_file_t *fp, const char *path)
{
    return VFS_FUNC_ATTACH;
}

static void vfs_test_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    g_vfs_func_entry = 1;
}

static int32_t vfs_test_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    return VFS_FUNC_ATTACH;
}

static void vfs_test_seekdir(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc)
{
    g_vfs_func_entry = 1;
}

static int32_t vfs_test_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *suf)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_access(vfs_file_t *fp, const char *path, int32_t amode)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_pathconf(vfs_file_t *fp, const char *path, int name)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_fpathconf(vfs_file_t *fp, int name)
{
    return VFS_FUNC_ATTACH;
}

static int32_t vfs_test_utime(vfs_file_t *fp, const char *path, const vfs_utimbuf_t *times)
{
    return VFS_FUNC_ATTACH;
}

static vfs_filesystem_ops_t vfs_test_ops = {
    .open      = &vfs_test_open,
    .close     = &vfs_test_close,
    .read      = &vfs_test_read,
    .write     = &vfs_test_write,
    .lseek     = &vfs_test_lseek,
    .sync      = &vfs_test_sync,
    .stat      = &vfs_test_stat,
    .fstat     = &vfs_test_fstat,
    .link      = &vfs_test_link,
    .unlink    = &vfs_test_unlink,
    .remove    = &vfs_test_remove,
    .rename    = &vfs_test_rename,
    .opendir   = &vfs_test_opendir,
    .readdir   = &vfs_test_readdir,
    .closedir  = &vfs_test_closedir,
    .mkdir     = &vfs_test_mkdir,
    .rmdir     = &vfs_test_rmdir,
    .rewinddir = &vfs_test_rewinddir,
    .telldir   = &vfs_test_telldir,
    .seekdir   = &vfs_test_seekdir,
    .ioctl     = &vfs_test_ioctl,
    .statfs    = &vfs_test_statfs,
    .access    = &vfs_test_access,
    .pathconf  = &vfs_test_pathconf,
    .fpathconf = &vfs_test_fpathconf,
    .utime     = &vfs_test_utime,
};

static void test_vfs_case(void)
{
    int32_t fd  = 0;
    int32_t ret = 0;

    char *path  = "/VFS/abcd0";
    char *path2 = "/VFS/efgh0";

    vfs_dir_t *dir;

    ret = vfs_register_fs("/VFS", &vfs_test_ops, NULL);
    YUNIT_ASSERT(ret == 0)

    fd = vfs_open(path, 0);

    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_lseek(fd, 0, 0));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_sync(fd));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_read(fd, NULL, 0));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_write(fd, NULL, 0));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_fstat(fd, NULL));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_fpathconf(fd, 0))

    YUNIT_ASSERT(0 == vfs_close(fd));

    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_stat(path, NULL));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_link(path, path2));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_unlink(path));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_remove(path));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_rename(path, path2));

    g_vfs_func_entry = 0;
    dir = vfs_opendir(path);
    YUNIT_ASSERT(g_vfs_func_entry == 1);

    g_vfs_func_entry = 0;
    vfs_readdir(dir);
    YUNIT_ASSERT(g_vfs_func_entry == 1);

    g_vfs_func_entry = 0;
    vfs_rewinddir(dir);
    YUNIT_ASSERT(g_vfs_func_entry == 1);

    g_vfs_func_entry = 0;
    vfs_seekdir(dir, 0);
    YUNIT_ASSERT(g_vfs_func_entry == 1);

    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_telldir(dir));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_closedir(dir));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_mkdir(path));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_rmdir(path));

    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_statfs(path, NULL));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_access(path, 0));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_pathconf(path, 0));
    YUNIT_ASSERT(VFS_FUNC_ATTACH == vfs_utime(path, NULL));

    ret = vfs_unregister_fs("/VFS");
    YUNIT_ASSERT(ret == 0)
}

static yunit_test_case_t aos_vfs_testcases[] = {
    { "fs_register", test_vfs_case},
    YUNIT_TEST_CASE_NULL
};

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

static yunit_test_suite_t suites[] = {
    { "vfs", init, cleanup, setup, teardown, aos_vfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_vfs(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_vfs);


