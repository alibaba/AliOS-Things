/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "plat_gen.h"
#include "plat_comm.h"

#define DEV_ID_LEN 36
#define FLASH_BLOCK_LEN 2048

#if CONFIG_SUDO_SUPPORT
#define PROV_DATA_PATH "/usr/.security/.dev_key"
#define DIR_NAME "/usr/.security"
#else

#ifdef PLATFORM_ANDROID
#define PROV_DATA_PATH "/sdcard/Android/data/.security/.dev_key"
#define DIR_NAME "/sdcard/Android/data/.security"
#else
#define PROV_DATA_PATH "./.security/.dev_key"
#define DIR_NAME "./.security"
#endif
#endif

#define FILE_NAME PROV_DATA_PATH

#define pl_memcpy memcpy
#define pl_memset memset
#define pl_malloc malloc
#define pl_free   free

static int _init_rsvd_part()
{
    //if no rserved partition just create km file
    DIR *dir;
    size_t file_len = 0;
    uint8_t *flash_block = NULL;
    int fd = 0;
    int fret = 0;
    int ret = 0;

    dir = opendir((char *)DIR_NAME);
    if (NULL == dir) {
        fret = mkdir(DIR_NAME, S_IRWXU |
                     S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH);
        if (fret < 0) {
            PL_ERR("mkdir failed errno is %d\n", errno);
            return -1;
        }

#ifndef PLATFORM_ANDROID
        if (chmod(DIR_NAME, S_IRWXU |
                  S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH)) {
            PL_ERR("chmod failed errno is %d\n", errno);
            closedir(dir);
            return -1;
        }
#endif
    } else {
        closedir(dir);
    }

    //file has already exist
    if (access(FILE_NAME, F_OK) == 0) {
        PL_INF("file has already exist\n");
        return 0;
    }

    //create FLASH_BLOCK_LEN file
    fd = open(FILE_NAME, O_CREAT|O_RDWR, S_IRWXU | S_IRGRP | S_IROTH);
    if (fd == -1) {
        PL_ERR("open file failed errno %d\n", errno);
        return -1;
    }

#ifndef PLATFORM_ANDROID
    if (fchmod(fd, S_IRWXU | S_IRGRP | S_IROTH)) {
        PL_ERR("file chmod failed errno is %d\n", errno);
        close(fd);
        return -1;
    }
#endif

    flash_block = (uint8_t *)pl_malloc(FLASH_BLOCK_LEN);
    if (!flash_block) {
        PL_ERR("malloc failed\n");
        ret = -1;
        goto clean;
    }

    pl_memset(flash_block, 0, FLASH_BLOCK_LEN);

    //fix file length first FLASH_BLOCK_LEN for km
    file_len = write(fd, flash_block, FLASH_BLOCK_LEN);
    if (file_len != FLASH_BLOCK_LEN) {
        PL_ERR("seek failed errno %d\n", errno);
        ret = -1;
        goto clean;
    }

    //fix file length last FLASH_BLICK_LEN for prov
    file_len = write(fd, flash_block, FLASH_BLOCK_LEN);
    if (file_len != FLASH_BLOCK_LEN) {
        PL_ERR("seek failed errno %d\n", errno);
        ret = -1;
        goto clean;
    }

clean:
    close(fd);
    if (flash_block) {
        pl_free(flash_block);
        flash_block = NULL;
    }

    return ret;
}

#if 0
#define cpuid(in,a,b,c,d)
asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

int get_dev_id(uint8_t *dev_id, uint32_t *id_len)
{
    int i = 0;
    unsigned long eax, ebx, ecx, edx;
    unsigned long maxi, unused;

    if (*id_len < DEV_ID_LEN) {
        PL_ERR("short buffer id len is %d\n", *id_len);
        *id_len = DEV_ID_LEN;
        return -1;
    }

    cpuid (0, maxi, unused, unused, unused);
    maxi &= 0xffff;

    if (maxi < 3) {
        PL_ERR("not support get cpuid\n");
        return -1;
    }

    cpuid (1, eax, ebx, ecx, edx);

    dev_id[0] = (eax & 0xff000000) >> 24;
    dev_id[1] = (eax & 0x00ff0000) >> 16;
    dev_id[2] = (eax & 0x0000ff00) >> 8;
    dev_id[3] = eax & 0x000000ff;

    return 0;
}
#endif


int get_dev_id(uint8_t *dev_id, uint32_t *id_len)
{
    int ret = 0;

    if (*id_len < DEV_ID_LEN) {
        PL_ERR("short buffer id len is %d\n", *id_len);
        *id_len = DEV_ID_LEN;
        return -1;
    }

#if 0
    size_t read_len = 0;
    int fd = -1;
    fd = open("/sys/class/dmi/id/product_uuid", O_RDONLY);
    if (fd == -1) {
        PL_ERR("open file failed errno %d\n", errno);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);
    read_len = read(fd, dev_id, DEV_ID_LEN);
    if (read_len != DEV_ID_LEN) {
        PL_ERR("read failed errno %d\n", errno);
        ret = -1;
        goto clean;
    }

    *id_len = DEV_ID_LEN;

clean:
    close(fd);
#else
    char *product_uuid = "12345678-1234-5678-ABCD-CBA987654321";
    pl_memcpy(dev_id, product_uuid, DEV_ID_LEN);
    *id_len = DEV_ID_LEN;
#endif

    return ret;
}

int open_rsvd_part(int flag)
{
    int fd = -1;
    int ret = 0;

    //check if file has already exist
    if (access(FILE_NAME, F_OK)) {
        ret = _init_rsvd_part();
        if (ret) {
            PL_ERR("init rsvd part failed\n");
            return -1;
        }
    }

    if (RO_READ == flag) {
        fd = open(PROV_DATA_PATH, O_RDONLY);;
    } else if (RO_WRITE == flag) {
        fd = open(PROV_DATA_PATH, O_WRONLY);
    } else if ((RO_READ | RO_WRITE) == flag) {
        fd = open(PROV_DATA_PATH, O_RDWR, S_IRUSR|S_IWUSR);
    } else {
        PL_ERR("not support flag\n");
        return -1;
    }

    if (fd == -1) {
        PL_ERR("open file failed errno %d\n", errno);
        return -1;
    }

    return fd;
}

int write_rsvd_part(int fd, uint32_t offset, void *data, uint32_t data_len)
{
    uint32_t write_len = 0;

    if (fd < 0 || (data_len != 0 && data == NULL)) {
        PL_ERR("bad param \n");
        return -1;
    }

    if (data_len == 0) {
        return 0;
    }

    lseek(fd, offset, SEEK_SET);
    write_len = write(fd, data, data_len);
    if (write_len != data_len) {
        PL_ERR("write failed errno is %d\n", errno);
        return -1;
    }

    return 0;
}

int read_rsvd_part(int fd, uint32_t offset, void *buffer, uint32_t read_len)
{
    uint32_t real_read_len = 0;

    if (fd < 0) {
        PL_ERR("bad params fd\n");
        return -1;
    }

    lseek(fd, offset, SEEK_SET);

    real_read_len = read(fd, buffer, read_len);
    if (real_read_len != read_len) {
        PL_ERR("read failed real read len is %d\n", real_read_len);
        return -1;
    }

    return 0;
}

int close_rsvd_part(int fd)
{
    return close(fd);
}

