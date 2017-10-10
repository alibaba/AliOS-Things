/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <errno.h>
#include <hal/soc/flash.h>
#include <vfs_inode.h>
#include <vfs_register.h>

#define SECTOR_SIZE 0x1000
#define SECTOR_MASK ~(SECTOR_SIZE - 1)

static int flash_open(inode_t *node, file_t *file)
{
    return 0;
}

static ssize_t flash_write(file_t *f, const void *buf, size_t len)
{
    int pno = (int)(long)f->node->i_arg;
    uint32_t offset, sector_off;
    int ret;
    size_t write_size;

    sector_off = f->offset & SECTOR_MASK;
    write_size = (f->offset - sector_off + len + (~SECTOR_MASK)) & SECTOR_MASK;

    void *buffer = (void *)aos_malloc(write_size);
    if (!buffer) {
        return 0;
    }
    memset(buffer, 0, write_size);

    offset = sector_off;
    ret = hal_flash_read(pno, &offset, buffer, write_size);
    if (ret < 0) {
        goto exit;
    }

    memcpy(buffer + (f->offset) - sector_off, buf, len);

    offset = sector_off;
    ret = hal_flash_erase(pno, offset, write_size);
    if (ret < 0) {
        goto exit;
    }

    ret = hal_flash_write(pno, &offset, buffer, write_size);
    if (ret < 0) {
        goto exit;
    }

    if ((offset - sector_off) == write_size) {
        f->offset += len;
        ret = len;
    }

exit:
    aos_free(buffer);
    return ret;
}

static ssize_t flash_read(file_t *f, void *buf, size_t len)
{
    int pno = (int)(long)f->node->i_arg;
    uint32_t offset = f->offset;
    int ret;

    ret = hal_flash_read(pno, &f->offset, buf, len);

    if (ret < 0) {
        return 0;
    }

    return f->offset - offset;
}

static file_ops_t flash_fops = {
    .open = flash_open,
    .read = flash_read,
    .write = flash_write,
};

static int vflash_register_partition(int pno)
{
    char pname[32];
    int ret;

    snprintf(pname, sizeof(pname) - 1, "/dev/flash%d", pno);
    ret = aos_register_driver(pname, &flash_fops, (void *)(long)pno);

    return ret;
}

#define APP_TAG "vflash"
#define VFLASH_PARTITION    99

static const char *dev_name = "/dev/flash99";
static const char *t_string_1 = "t_str_1";

static int vflash_write_testcase()
{
    int fd, ret = 0;

    fd = aos_open(dev_name, O_RDWR);
    if (fd <= 0) {
        LOGE(APP_TAG, "device open failed.");
        return -EIO;
    }

    ret = aos_write(fd, t_string_1, strlen(t_string_1));
    if (ret != strlen(t_string_1)) {
        LOGE(APP_TAG, "partition write failed.");
        return ret;
    }

    ret = aos_close(fd);
    if (ret != 0) {
        LOGE(APP_TAG, "device close failed.");
        return ret;
    }

    return ret;
}

static int vflash_read_testcase()
{
    int fd, ret = 0;
    char buf[10] = {0};
 
    fd = aos_open(dev_name, O_RDONLY);
    if (fd <= 0) {
        LOGE(APP_TAG, "device open failed.");
        return -EIO;
    }

    ret = aos_read(fd, buf, strlen(t_string_1));
    if (ret != strlen(t_string_1)) {
        LOGE(APP_TAG, "partition read failed.");
        return ret;
    }
    if ((ret = memcmp(buf, t_string_1, strlen(t_string_1))) != 0) {
        LOGE(APP_TAG, "partition memcmp failed.");
        return ret;
    }

    ret = aos_close(fd);
    if (ret != 0) {
        LOGE(APP_TAG, "device close failed.");
        return ret;
    }

    return ret;
}

static int vflash_overwrite_testcase()
{
    int fd, ret = -1;
    char *buffer = NULL;
    char *cmp_buf = NULL;

    fd = aos_open(dev_name, O_RDWR);
    if (fd <= 0) {
        LOGE(APP_TAG, "device close failed");
        return -EIO;
    }

    buffer = (char *)aos_malloc(2 *SECTOR_SIZE);
    if (!buffer)
        return -ENOMEM;

    memset(buffer, 'a', 2000);
    ret = aos_write(fd, buffer, 2000);

    memset(buffer, 'b', 2256);
    ret = aos_write(fd, buffer, 2256);

    memset(buffer, 'c', 2 * SECTOR_SIZE);
    ret = aos_write(fd, buffer, 2 * SECTOR_SIZE);

    ret = aos_close(fd);
    
    cmp_buf = (char *)aos_malloc(2 * SECTOR_SIZE);
    if (!cmp_buf) {
        if (buffer)
            aos_free(buffer);
        return -ENOMEM;
    }

    fd = aos_open(dev_name, O_RDONLY);
    if (fd <= 0) {
        LOGE(APP_TAG, "device close failed");
        return -EIO;
    }


    memset(buffer, 0, 2 * SECTOR_SIZE);
    ret = aos_read(fd, buffer, 2000);
    memset(cmp_buf, 'a', 2000);
    ret = memcmp(buffer, cmp_buf, 2000);
    if (ret != 0) {
        LOGE(APP_TAG, "memcmp failed");
        return ret;
    }

    ret = aos_read(fd, buffer, 2256);
    memset(cmp_buf, 'b', 2256);
    ret = memcmp(buffer, cmp_buf, 2256);
    if (ret != 0) {
        LOGE(APP_TAG, "memcmp failed");
        return ret;
    }


    ret = aos_read(fd, buffer, 2 * SECTOR_SIZE);
    memset(cmp_buf, 'c', 2 * SECTOR_SIZE);
    ret = memcmp(buffer, cmp_buf, 2 * SECTOR_SIZE);
    if (ret != 0) {
        LOGE(APP_TAG, "memcmp failed");
        return ret;
    }


    aos_close(fd);
    if (buffer)
        aos_free(buffer);

    if (cmp_buf)
        aos_free(cmp_buf);

    return ret;

}



/* 
  * It is a test demo for flash read/write via VFS. 
  * Currently this example is only executed on linux simulation
  * If you want to execute on the board 
  * please ensure the vflash partition won't corrupt the system
  */
int application_start(int argc, char *argv[])
{
    int ret = -1;
    char fn[64] = {0};

    LOGW(APP_TAG, "vflash test demo starting!");
    ret = vflash_register_partition(VFLASH_PARTITION);
    if (ret != 0) {
        LOGE(APP_TAG, "partition register failed.");
        return ret;
    }

    ret = vflash_write_testcase();
    if (ret != 0)
        return ret;

    ret = vflash_read_testcase();
    if (ret != 0)
        return ret;

    ret = vflash_overwrite_testcase();
    if (ret != 0)
        return ret;

    ret = aos_unregister_driver(dev_name);

    LOGW(APP_TAG, "vflash test demo finished!");

    snprintf(fn, sizeof(fn), "rm -f ./aos_partition_*");
    system(fn);
 
    exit(0);

    return ret;
}

