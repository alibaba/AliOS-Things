/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "vfs_types.h"
#include "vfs_api.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static int32_t driver_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    printf("%s\n", __func__);
    return 0;
}

static int32_t driver_vfs_close(vfs_file_t *fp)
{
    printf("%s\n", __func__);
    return 0;
}

static int32_t driver_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    printf("%s\n", __func__);
    return 0;
}

static int32_t driver_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    printf("%s\n", __func__);
    return 0;
}

static int32_t driver_vfs_ioctl(vfs_file_t *fp, int32_t cmd, uint32_t arg)
{
    printf("%s\n", __func__);
    return 0;
}

static uint32_t driver_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    printf("%s\n", __func__);
    return 0;
}

vfs_file_ops_t driver_ops = {
    .open      = &driver_vfs_open,
    .close     = &driver_vfs_close,
    .read      = &driver_vfs_read,
    .write     = &driver_vfs_write,
    .ioctl     = &driver_vfs_ioctl,
    .lseek     = &driver_vfs_lseek,
};

static void driver_vfs_example(void)
{
    char *mount_path = "/dev";
    int ret = 0;
    char *buf[10];
    uint32_t len = 0;
    int fd = 0;
    int32_t cmd = 0;
    uint32_t arg = 0;
    int64_t off = 0;
    int32_t whence = 0;

    printf("driver vfs example start\r\n");

    ret = vfs_register_driver(mount_path, &driver_ops, NULL);
    if (ret < 0) {
        printf("vfs_register_driver failed!\n");
        return;
    }

    fd = vfs_open(mount_path, O_WRONLY);
    if (fd < 0) {
        printf("vfs_open failed!\n");
        vfs_unregister_driver(mount_path);
        return;
    }
    vfs_read(fd, buf, len);
    vfs_write(fd, buf, len);
    vfs_ioctl(fd, cmd, arg);
    vfs_lseek(fd, off, whence);

    vfs_close(fd);
    vfs_unregister_driver(mount_path);
    printf("driver vfs example end\r\n");
}

static int32_t fs_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    printf("%s\n", __func__);
    return 0;
}
static int32_t fs_vfs_close(vfs_file_t *fp)
{
    printf("%s\n", __func__);
    return 0;
}
static int32_t fs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    printf("%s\n", __func__);
    return 0;
}
static int32_t fs_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    printf("%s\n", __func__);
    return 0;
}
vfs_filesystem_ops_t fs_ops = {
    .open      = &fs_vfs_open,
    .close     = &fs_vfs_close,
    .read      = &fs_vfs_read,
    .write     = &fs_vfs_write,
};

static void fs_vfs_example(void)
{
    char *mount_path = "/fs";
    int ret = 0;
    char *buf[10];
    uint32_t len = 0;
    int fd = 0;

    printf("fs vfs example start\r\n");

    ret = vfs_register_fs(mount_path, &fs_ops, NULL);
    if (ret < 0) {
        printf("vfs_register_fs failed!\n");
        return;
    }

    fd = vfs_open(mount_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("vfs_open failed!\n");
        vfs_unregister_fs(mount_path);
        return;
    }

    vfs_read(fd, buf, len);
    vfs_write(fd, buf, len);
    vfs_close(fd);

    vfs_unregister_fs(mount_path);
    printf("fs vfs example end\r\n");
}

static void vfs_comp_example(int argc, char **argv)
{
    driver_vfs_example();
    fs_vfs_example();
    printf("vfs example test success!\r\n");
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(vfs_comp_example, vfs_example, vfs component base example)
#endif
