/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "aos/kernel.h"
#include "aos/hal/nand.h"

#define CONFIG_FLASH_START_ADDR 0
#define CONFIG_FLASH_PAGE_SIZE  512
#define CONFIG_FLASH_SPARE_SIZE 16
#define CONFIG_FLASH_BLOCK_SIZE 32
#define CONFIG_FLASH_ZONE_SIZE  128
#define CONFIG_FLASH_ZONE_NUM   1

static int nand_ll_init(int fd)
{
    int flash_size, nbytes = 0;

    flash_size = (CONFIG_FLASH_PAGE_SIZE + CONFIG_FLASH_SPARE_SIZE) *
                  CONFIG_FLASH_BLOCK_SIZE * CONFIG_FLASH_ZONE_SIZE * CONFIG_FLASH_ZONE_NUM;

    char *buffer = (char *)malloc(flash_size);
    if (!buffer)
        return -ENOMEM;

    memset(buffer, 0xff, flash_size);
    nbytes = pwrite(fd, buffer, flash_size, CONFIG_FLASH_START_ADDR);
    free(buffer);

    return nbytes < 0 ? nbytes : 0;

}

static int nand_open(int w)
{
    int fd;
    char path[64] = {0};
    snprintf(path, sizeof(path), "./aos_partition_%d_nand.bin", getpid());

    if (w)
        fd = open(path, O_RDWR);
    else
        fd = open(path, O_RDONLY);

    if (fd < 0) {
        umask(0111);
        fd = creat(path, S_IRWXU | S_IRWXG);
        if (fd < 0)
            return fd;
        nand_ll_init(fd);
        close(fd);
        fd = open(path, O_RDWR);
    }

    return fd;
}

int32_t hal_nand_init(nand_dev_t *nand)
{
    // to ensure the simulation file is created!
    int fd = nand_open(1);
    if (fd < 0)
        return -EIO;
    close(fd);

    nand->base_addr = CONFIG_FLASH_START_ADDR;
    nand->config.page_size = CONFIG_FLASH_PAGE_SIZE;
    nand->config.spare_area_size = CONFIG_FLASH_SPARE_SIZE;
    nand->config.block_size = CONFIG_FLASH_BLOCK_SIZE;
    nand->config.zone_size = CONFIG_FLASH_ZONE_SIZE;
    nand->config.zone_number = CONFIG_FLASH_ZONE_NUM;
    return 0;
}

int32_t hal_nand_finalize(nand_dev_t *nand)
{
    memset(nand, 0, sizeof(nand_dev_t));
    return 0;
}

int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
    int nbytes, location, full_page_size;
    int fd = nand_open(0);

    if (fd < 0) {
        return -EIO;
    }

    nbytes = 0;
    location = ((addr->zone * (nand->config.zone_size)) + addr->block) * (nand->config.block_size) + addr->page;
    full_page_size = nand->config.page_size + nand->config.spare_area_size;

    if (data) {
        while (page_count) {
            nbytes = pread(fd, data, nand->config.page_size, location * full_page_size);
            location++;
            page_count--;
        }
    }
    close(fd);    
    return nbytes < 0 ? nbytes : 0;
}

int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
    int nbytes, location, full_page_size;
    int fd = nand_open(1);

    if (fd < 0)
        return -EIO;

    nbytes = 0;
    location = ((addr->zone * (nand->config.zone_size)) + addr->block) * (nand->config.block_size) + addr->page;
    full_page_size = nand->config.page_size + nand->config.spare_area_size;

    if (data) {
        while (page_count) {
            nbytes = pwrite(fd, data, nand->config.page_size, location * full_page_size);
            location++;
            page_count--;
        }
    }
    close(fd);
    return nbytes < 0 ? nbytes : 0;
}

int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
    int nbytes, location, full_page_size;
    int fd = nand_open(0);

    if (fd < 0)
        return -EIO;

    nbytes = 0;
    location = ((addr->zone * (nand->config.zone_size)) + addr->block) * (nand->config.block_size) + addr->page;
    full_page_size = nand->config.page_size + nand->config.spare_area_size;

    if (data) {
        nbytes = pread(fd, data, nand->config.spare_area_size, location * full_page_size + nand->config.page_size);
    }

    close(fd);
    return nbytes < 0 ? nbytes : 0;
}

int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
    int nbytes, location, full_page_size;
    int fd = nand_open(1);

    if (fd < 0)
        return -EIO;

    nbytes = 0;
    location = ((addr->zone * (nand->config.zone_size)) + addr->block) * (nand->config.block_size) + addr->page;
    full_page_size = nand->config.page_size + nand->config.spare_area_size;

    if (data) {
        nbytes = pwrite(fd, data, nand->config.spare_area_size, location * full_page_size + nand->config.page_size);
    }

    close(fd);
    return nbytes < 0 ? nbytes : 0;

}

int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr)
{
    char *buffer;
    int nbytes, location, block_size, full_page_size;
    int fd = nand_open(1);

    if (fd < 0)
        return -EIO;

    nbytes = 0;
    location = ((addr->zone * (nand->config.zone_size)) + addr->block) * (nand->config.block_size);
    full_page_size = nand->config.page_size + nand->config.spare_area_size;
    block_size = (nand->config.block_size) * full_page_size;

    buffer = (char *)malloc(block_size);
    if (!buffer) {
        close(fd);
        return -EIO;
    }

    memset(buffer, 0xff, block_size);
    nbytes = pwrite(fd, buffer, block_size, location * full_page_size);

    close(fd);
    free(buffer);
    return nbytes < 0 ? nbytes : 0;
}
