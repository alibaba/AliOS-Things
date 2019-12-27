/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "ls_osa.h"
#include "ls_hal.h"
#include "plat_comm.h"

#define FLASH_BLOCK_LEN 2048
#define DEV_ID_LEN 6
#define PROV_DATA_PATH "./.security/.dev_key"

#define DIR_NAME "./.security"
#define FILE_NAME PROV_DATA_PATH

#define pl_memcpy memcpy
#define pl_memset memset
#define pl_malloc ls_osa_malloc
#define pl_free   ls_osa_free

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

        if (chmod(DIR_NAME, S_IRWXU |
                  S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH)) {
            PL_ERR("chmod failed errno is %d\n", errno);
            closedir(dir);
            return -1;
        }
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

    if (fchmod(fd, S_IRWXU | S_IRGRP | S_IROTH)) {
        PL_ERR("file chmod failed errno is %d\n", errno);
        close(fd);
        return -1;
    }


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

int ls_hal_get_dev_id(uint8_t *dev_id, uint32_t *id_len)
{
    int i = 0;

    if (*id_len < DEV_ID_LEN) {
        PL_ERR("short buffer id len is %d\n", *id_len);
        *id_len = DEV_ID_LEN;
        return -1;
    }

    for (i = 0; i < DEV_ID_LEN; i++) {
        dev_id[i] = i;
    }

    *id_len = DEV_ID_LEN;

    return 0;
}

int ls_hal_open_rsvd_part(int flag)
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

    if (LS_HAL_READ == flag) {
        fd = open(PROV_DATA_PATH, O_RDONLY);;
    } else if (LS_HAL_WRITE == flag) {
        fd = open(PROV_DATA_PATH, O_WRONLY);
    } else if ((LS_HAL_READ | LS_HAL_WRITE) == flag) {
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

int ls_hal_write_rsvd_part(int fd, uint32_t offset, void *data, uint32_t data_len)
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

int ls_hal_read_rsvd_part(int fd, uint32_t offset, void *buffer, uint32_t read_len)
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

int ls_hal_close_rsvd_part(int fd)
{
    return close(fd);
}

