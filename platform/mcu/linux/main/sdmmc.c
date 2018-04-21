/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <hal/soc/sd.h>

#define SD_BLOCK_SIZE 512

static int sdmmc_open(int w)
{
    int fd;
    char path[64] = {0};
    snprintf(path, sizeof(path), "./aos_partition_%d_sdcard.bin", getpid());

    if (w)
        fd = open(path, O_RDWR);
    else
        fd = open(path, O_RDONLY);

    if (fd < 0) {
        umask(0111);
        fd = creat(path, S_IRWXU | S_IRWXG);
        if (fd < 0)
            return fd;
        close(fd);
        fd = open(path, O_RDWR);
    }

    return fd;
}

static int sdmmc_read(uint8_t *buff, uint32_t sector, uint32_t count)
{
    int fd = sdmmc_open(0);
    if (fd < 0)
        return -EIO;

    int ret = pread(fd, buff, (count * SD_BLOCK_SIZE), (sector * SD_BLOCK_SIZE));
    close(fd);

    if (ret < 0)
        return -EIO;
    else
        return 0;
}

static int sdmmc_write(const char *buff, uint32_t sector, uint32_t count)
{
    int fd = sdmmc_open(1);
    if (fd < 0)
        return -EIO;

    int ret = pwrite(fd, buff, (count * SD_BLOCK_SIZE), (sector * SD_BLOCK_SIZE));
    close(fd);

    if (ret < 0)
        return -EIO;
    else
        return 0;
}

int32_t hal_sd_init(sd_dev_t *sd)
{
    return 0;
}

int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr, uint32_t blks, uint32_t timeout)
{
    return sdmmc_read(data, blk_addr, blks);
}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr, uint32_t blks, uint32_t timeout)
{
    return sdmmc_write(data, blk_addr, blks);
}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    return 0;
}

int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
    *stat = SD_STAT_TRANSFER;
    return 0;
}

int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
    info->blk_nums = 256;
    info->blk_size = SD_BLOCK_SIZE;
    return 0;
}

int32_t hal_sd_finalize(sd_dev_t *sd)
{
    return 0;
}
