/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aos/cli.h>
#include <aos/vfs.h>
#include <hal2vfs/io_ramdrv.h>
#include <drivers/u_ld.h>
#include <drivers/char/u_device.h>

#define MAX_BUFFER_SIZE 1024 * 1024
#define MAX_OPEN_FILES 16
static int g_ramdev_fd[4] = {-1, -1, -1, -1};
static int g_ramdev_multi_access_fd[MAX_OPEN_FILES] = {-1};

int vfs_ramdrv_test(int index)
{
    int i = 0;
    int ret = 0;
    int fd = 0;
    char name[16] = {0};
    io_ramdrv_arg_t ramdrv_arg;

    snprintf(name, sizeof(name), "/dev/ramdrv%d", index);
    ddkc_info("opening name:%s\r\n", name);
    fd = open(name, 0);
    ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        memset(&ramdrv_arg, 0, sizeof(ramdrv_arg));
        for (i = 0; i < sizeof(ramdrv_arg.wbuf); i++) {
            ramdrv_arg.wbuf[i] = i;
        }
        ret = lseek(fd, 1000, SEEK_SET);
        ret += lseek(fd, 0, SEEK_SET);
        if(ret < 0) {
            ddkc_warn("lseek failed:%d\r\n", ret);
            goto err;
        }

        ddkc_info("ioctl on %s with cmd:%d, arg:%p\r\n", name, IOC_RAMDRV_LOOPBACK, &ramdrv_arg);
        ret = ioctl(fd, IOC_RAMDRV_LOOPBACK, (unsigned long)&ramdrv_arg);
        ddkc_info("ioctl on %s with cmd:%d return %d\r\n", name, IOC_RAMDRV_LOOPBACK, ret);
        if (!memcmp(ramdrv_arg.wbuf , ramdrv_arg.rbuf, sizeof(ramdrv_arg.rbuf))) {
            ddkc_info("ioctl success, wbuf equals with rbuf\r\n");
            ret = 0;
        } else {
            ddkc_info("ioctl fail, wbuf not equals with rbuf\r\n");
            ret = -1;
        }
    } else
        ret = -1;

err:
    if (fd >= 0) {
        ddkc_info("closing name:%s\r\n", name);
        close(fd);
    }
    return ret;
}

static void vfs_ramdrv_ioctl_cmd(char *buf, int len, int argc, char **argv)
{
    int ret = 0;
    int index = argc > 1 ? atoi(argv[1]) : 1;
    int loop = argc > 2 ? atoi(argv[2]) : 1;

    ddkc_info("argc:%d, index:%d\r\n", argc, index);
    while (loop--) {
       ret = vfs_ramdrv_test(index);
       if (ret) {
           ddkc_err("ioctl test failed, ret:%d\r\n", ret);
           break;
       }
    }

    ddkc_info("vfs ramdrv test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

static void vfs_ramdrv_close_cmd(char *wbuf, int len, int argc, char **argv)
{
    char name[16] = {0};
    int index = argc > 1 ? atoi(argv[1]) : 1;

    if (index > 3 || index < 0) {
        ddkc_err("invalid index:%d, should between [0, 3]\r\n", index);
        return;
    }

    snprintf(name, sizeof(name), "/dev/ramdrv%d", index);

    if (g_ramdev_fd[index] < 0) {
        ddkc_warn("%s not opened yet, fd:%d\r\n", name, g_ramdev_fd[index]);
        return;
    }

    ddkc_info("closing name:%s\r\n", name);
    close(g_ramdev_fd[index]);
    g_ramdev_fd[index] = -1;

    return;
}

static void vfs_ramdrv_open_cmd(char *wbuf, int len, int argc, char **argv)
{
    int fd = -1;
    char name[16] = {0};
    int index = argc > 1 ? atoi(argv[1]) : 1;

    if (index > 3 || index < 0) {
        ddkc_err("invalid index:%d, should between [0, 3]\r\n", index);
        return;
    }

    snprintf(name, sizeof(name), "/dev/ramdrv%d", index);

    if (g_ramdev_fd[index] >= 0) {
        ddkc_warn("%s already opened, fd:%d, close it now\r\n", name, g_ramdev_fd[index]);
        close(g_ramdev_fd[index]);
        g_ramdev_fd[index] = -1;
    }

    ddkc_info("opening name:%s\r\n", name);
    fd = open(name, 0);
    ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);
    if (fd >= 0) {
        g_ramdev_fd[index] = fd;
    }

    return;
}

static void vfs_ramdrv_multi_open_cmd(char *wbuf, int len, int argc, char **argv)
{
    int i = 0;
    int j = 0;
    int fd = -1;
    char name[16] = {0};
    int index = argc > 1 ? atoi(argv[1]) : 1;
    int count = argc > 2 ? atoi(argv[2]) : 17;

    for (j = 0; j < count; j++) {
        for (i = 0; i < MAX_OPEN_FILES; i++) {
            if (g_ramdev_multi_access_fd[i] < 0)
                break;
        }
        if (i >= MAX_OPEN_FILES) {
            ddkc_err("too many file is opened, closing them\r\n");
            for (i = 0; i < MAX_OPEN_FILES; i++) {
                ddkc_info("closing %d\r\n", g_ramdev_multi_access_fd[i]);
                close(g_ramdev_multi_access_fd[i]);
                g_ramdev_multi_access_fd[i] = -1;
            }
            i = 0;
        }

        snprintf(name, sizeof(name), "/dev/ramdrv%d", index);
        ddkc_info("opening name:%s\r\n", name);
        fd = open(name, 0);
        ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);
        if (fd >= 0) {
            g_ramdev_multi_access_fd[i] = fd;
        } else {
            ddkc_warn("multi open test on %s failed\r\n", name);
            break;
        }
    }

    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (g_ramdev_multi_access_fd[i] >= 0) {
            close(g_ramdev_multi_access_fd[i]);
            g_ramdev_multi_access_fd[i] = -1;
        }
    }

    return;
}

static void vfs_ramdrv_write_cmd(char *wbuf, int len, int argc, char **argv)
{
    int i = 0;
    int ret = 0;
    int length = argc > 1 ? atoi(argv[1]) : 1000;
    int loop = argc > 2 ? atoi(argv[2]) : 10;
    char *name = "/dev/ramdrv3";
    int fd = -1;
    char *buf = NULL;

    if (length > MAX_BUFFER_SIZE)
        length = MAX_BUFFER_SIZE;
    if (loop > MAX_BUFFER_SIZE)
        loop = MAX_BUFFER_SIZE;

    ddkc_info("argc:%d, length:%d, loop:%d\r\n", argc, length, loop);
    
    buf = malloc(length);
    if (!buf) {
        ddkc_err("malloc failed, length:%d\r\n", length);
        return;
    }
    memset(buf, RAMDRV_MAGIC_NUM, length);

    ddkc_info("opening name:%s\r\n", name);
    fd = open(name, 0);
    ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        for (i = 1; i < loop; i++) {
            ret = write(fd, buf, i);
            if (ret != i) {
                ddkc_err("write fails, [i:%d] != [ret:%d]\r\n", i, ret);
                break;
            }
            ddkc_info("write success, [i:%d] == [ret:%d]\r\n", i, ret);
        }
    }

    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    if (i != loop) {
        ddkc_err("write test failed, [loop:%d] != [i:%d]\r\n", loop, i);
        ret = -1;
    } else
        ret = 0;
    free(buf);
    ddkc_info("vfs ramdrv write test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

static void vfs_ramdrv_read_cmd(char *rbuf, int len, int argc, char **argv)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int length = argc > 1 ? atoi(argv[1]) : 1000;
    int loop = argc > 2 ? atoi(argv[2]) : 10;
    char *name = "/dev/ramdrv3";
    int fd = -1;
    char *buf = NULL;

    if (length > MAX_BUFFER_SIZE)
        length = MAX_BUFFER_SIZE;
    if (loop > MAX_BUFFER_SIZE)
        loop = MAX_BUFFER_SIZE;

    ddkc_info("argc:%d, length:%d, loop:%d\r\n", argc, length, loop);
    
    buf = malloc(length);
    if (!buf) {
        ddkc_err("malloc failed, length:%d\r\n", length);
        return;
    }
    memset(buf, RAMDRV_MAGIC_NUM + 0x11, length);

    ddkc_info("opening name:%s\r\n", name);
    fd = open(name, 0);
    ddkc_warn("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        for (i = 1; i < loop; i++) {
            ret = read(fd, buf, i);
            if (ret != i) {
                ddkc_err("read fails, [i:%d] != [ret:%d]\r\n", i, ret);
                break;
            }
            for (j = 0; j < i; j++) {
                if(buf[j] != RAMDRV_MAGIC_NUM) {
                    break;
                }
            }
            if (j != i) {
                ddkc_err("memcmp failed, [j:%d] != [i:%d]\r\n", j, i);
                break;
            }
            ddkc_info("memcmp succeed, [j:%d] === [i:%d]\r\n", j, i);
            memset(buf, RAMDRV_MAGIC_NUM + 0x11, length);
            ddkc_info("buf[0]:0x%x, magic num:0x%x\r\n", buf[0] & 0xff, RAMDRV_MAGIC_NUM);
        }
    }
    if (i != loop) {
        ddkc_err("read test failed, [loop:%d] != [i:%d]\r\n", loop, i);
        ret = -1;
    } else
        ret = 0;
    free(buf);

    if (fd >= 0){
        close(fd);
        fd = -1;
    }

    ddkc_info("vfs ramdrv read test %s, ret:%d\r\n", ret ? "failed" : "success", ret);

    return;
}

struct cli_command vfs_ramdrv_test_cmds[] = {
    {"ramit",         "ramdrv ioctl test", vfs_ramdrv_ioctl_cmd},
    {"ramwt",         "ramdrv write test", vfs_ramdrv_write_cmd},
    {"ramrt",         "ramdrv read test", vfs_ramdrv_read_cmd},
    {"ramot",         "ramdrv open test", vfs_ramdrv_open_cmd},
    {"rammot",         "ramdrv multiple open test", vfs_ramdrv_multi_open_cmd},
    {"ramct",         "ramdrv close test", vfs_ramdrv_close_cmd},
};

int vfs_ramdrv_test_cmd_init(void) {
    int i = 0;

    for (i = 0; i < MAX_OPEN_FILES; i++) {
        g_ramdev_multi_access_fd[i] = -1;
    }

    return aos_cli_register_commands(&vfs_ramdrv_test_cmds[0],
                                     sizeof(vfs_ramdrv_test_cmds)/sizeof(vfs_ramdrv_test_cmds[0]));
}
#ifndef AOS_KERNEL_MODE
POST_DRIVER_ENTRY(vfs_ramdrv_test_cmd_init)
#endif
