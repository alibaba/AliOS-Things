/*
 * Linux user-space block device wrapper
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "nftl_bd.h"
#include "inc/nftl_conf.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <time.h>
#include <string.h>

#define MAIN_AREA_SIZE (NFTL_BLOCK_SIZE * NFTL_BLOCK_NUMS)
#define BLOCK_SPARE_AREA_SIZE (SPARE_AREA_BYTES * NFTL_PAGE_PER_BLOCK_NUMS)
#define TOTAL_SPARE_AREA_SIZE (SPARE_AREA_BYTES * NFTL_PAGE_PER_BLOCK_NUMS * NFTL_BLOCK_NUMS)
#define NAND_TOTAL_SIZE (MAIN_AREA_SIZE + TOTAL_SPARE_AREA_SIZE)
#define BAD_THRESHOLD 100000
#define BAD_THRESHOLD_RANDOM 2000

uint8_t _erased_block[NFTL_BLOCK_SIZE];
uint8_t file_buffer[NAND_TOTAL_SIZE];
static uint32_t _op_cnt[NFTL_BLOCK_NUMS];

// Block device wrapper for user-space block devices
int nftl_bd_create(struct nftl_config *cfg, const char *path) {
    size_t filelen = 0;
    off_t err;
    ssize_t res;
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);

    int fd = open(path, O_RDWR|O_CREAT, 0666);
    if (fd < 0) {
        printf("Failed to open nftl device file, errno: %d\n", errno);
        return -errno;
    }

    err = lseek(fd, 0, SEEK_END);
    if (err < 0) {
        printf("Failed lseek nftl device file, errno: %d\n", errno);
        return -errno;
    }

    filelen = (size_t)err;
    if (filelen != NAND_TOTAL_SIZE && filelen != 0) {
        printf("nftl file len not valid: %d\n", (int)filelen);
        return -1;
    }

    memset(file_buffer, 0xff, sizeof(file_buffer));
    if (filelen > 0) {
        // read the whole file
        err = lseek(fd, 0, SEEK_SET);
        if (err < 0) {
            printf("%s %d Failed lseek nftl device file, errno: %d\n",
                   __func__, __LINE__, errno);
            return -errno;
        }

        res = read(fd, file_buffer, filelen);
        if (res < 0) {
            printf("failed to read nftl partition: %d, errno: %d\n", (int)res, errno);
            return -errno;
        }
    } else {
        // go to block
        err = lseek(fd, 0, SEEK_SET);
        if (err < 0) {
            printf("%s %d Failed to ioctl nftl device file, errno: %d\n", __func__, __LINE__, errno);
            return -errno;
        }

        // write block
        res = write(fd, file_buffer, sizeof(file_buffer));
        if (res < 0) {
            printf("%s %d Failed to write nftl device file, errno: %d\n", __func__, __LINE__, errno);
            return -errno;
        }
    }

    cfg->context = (void*)(intptr_t)fd;

    // get sector size
    if (!cfg->block_size) {
        long ssize;
        int err = ioctl(fd, BLKSSZGET, &ssize);
        if (err) {
            printf("Failed to ioctl nftl device file, errno: %d\n", errno);
            return -errno;
        }
        cfg->block_size = ssize;
    }

    // get size in sectors
    if (!cfg->block_count) {
        long size;
        int err = ioctl(fd, BLKGETSIZE, &size);
        if (err) {
            printf("%d Failed to ioctl nftl device file, errno: %d\n", __LINE__, errno);
            return -errno;
        }
        cfg->block_count = size;
    }

    // setup function pointers
    cfg->read  = nftl_bd_read;
    cfg->prog  = nftl_bd_prog;
    cfg->erase = nftl_bd_erase;
    cfg->sync  = nftl_bd_sync;

    memset(_erased_block, 0xff, sizeof(_erased_block));
    printf("nftl block device created/opened\r\n");

    return 0;
}

void nftl_bd_destroy(const struct nftl_config *cfg) {
    int fd = (intptr_t)cfg->context;
    close(fd);
    printf("nftl block device closed\r\n");
}

int nftl_bd_read(const struct nftl_config *cfg, nftl_block_t block,
               nftl_off_t off, void *buffer, nftl_size_t size) {
    int fd = (intptr_t)cfg->context;

    // check if read is valid
    assert(block < cfg->block_count);
    assert(size <= MAIN_AREA_SIZE);

    memcpy(buffer, file_buffer + (off_t)block*cfg->block_size + (off_t)off, size);

#if 0
    // go to block
    off_t err = lseek(fd, (off_t)block*cfg->block_size + (off_t)off, SEEK_SET);
    if (err < 0) {
        return -errno;
    }

    // read block
    ssize_t res = read(fd, buffer, (size_t)size);
    if (res < 0) {
        return -errno;
    }
#endif

    return 0;
}

int nftl_bd_read_with_spare(const struct nftl_config *cfg, nftl_block_t block,
                            nftl_off_t off, void *buffer, nftl_size_t size,
                            void *spare_buffer, nftl_size_t spare_size) {
    int pageidx = off / NFTL_PAGE_SIZE;
    int fd = (intptr_t)cfg->context;

    // check if read is valid
    assert(block < cfg->block_count);
    assert(size <= MAIN_AREA_SIZE);

    memcpy(buffer, file_buffer + (off_t)block*cfg->block_size + (off_t)off, size);
    memcpy(spare_buffer, file_buffer + MAIN_AREA_SIZE +
           (off_t)block*BLOCK_SPARE_AREA_SIZE +
           pageidx * SPARE_AREA_BYTES, spare_size);

#if 0
    // go to block
    off_t err = lseek(fd, (off_t)block*cfg->block_size + (off_t)off, SEEK_SET);
    if (err < 0) {
        return -errno;
    }

    // read block
    ssize_t res = read(fd, buffer, (size_t)size);
    if (res < 0) {
        return -errno;
    }
#endif

    return 0;
}

int nftl_bd_prog(const struct nftl_config *cfg, nftl_block_t block,
               nftl_off_t off, const void *buffer, nftl_size_t size) {
    int fd = (intptr_t)cfg->context;

    // check if write is valid
    assert(block < cfg->block_count);
    assert(size <= MAIN_AREA_SIZE);

    if (nftl_bd_isbad(cfg, block)) return -1; // bad block

    // simulate bad block, near threshold point but give some random
    _op_cnt[block]++;
    if (_op_cnt[block] > (BAD_THRESHOLD + rand() % BAD_THRESHOLD_RANDOM)) {
        printf("%s Block %d failed at %d-th operation.\n", __func__, block, _op_cnt[block]);
        return -1;
    }

    memcpy(file_buffer + (off_t)block*cfg->block_size + (off_t)off, buffer, size);

    // go to block
    off_t err = lseek(fd, 0, SEEK_SET);
    if (err < 0) {
        printf("%s %d Failed to ioctl nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    // write block
    ssize_t res = write(fd, file_buffer, sizeof(file_buffer));
    if (res < 0) {
        printf("%s %d Failed to write nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    return 0;
}

int nftl_bd_prog_with_spare(const struct nftl_config *cfg, nftl_block_t block,
                            nftl_off_t off, const void *buffer, nftl_size_t size,
                            const void *spare_buffer, nftl_size_t spare_size) {
    int pageidx = off / NFTL_PAGE_SIZE;
    int fd = (intptr_t)cfg->context;

    // check if write is valid
    assert(block < cfg->block_count);
    assert(size <= MAIN_AREA_SIZE);

    if (nftl_bd_isbad(cfg, block)) return -1; // bad block

    // simulate bad block, near threshold point but give some random
    _op_cnt[block]++;
    if (_op_cnt[block] > (BAD_THRESHOLD + rand() % BAD_THRESHOLD_RANDOM)) {
        printf("%s Block %d failed at %d-th operation.\n", __func__, block, _op_cnt[block]);
        return -1;
    }

    memcpy(file_buffer + (off_t)block*cfg->block_size + (off_t)off, buffer, size);
    memcpy(file_buffer + MAIN_AREA_SIZE +
           (off_t)block*BLOCK_SPARE_AREA_SIZE +
           pageidx * SPARE_AREA_BYTES,
           spare_buffer, spare_size);

    // go to block
    off_t err = lseek(fd, 0, SEEK_SET);
    if (err < 0) {
        printf("%s %d Failed to ioctl nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    // write block
    ssize_t res = write(fd, file_buffer, sizeof(file_buffer));
    if (res < 0) {
        printf("%s %d Failed to write nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    return 0;
}

int nftl_bd_erase(const struct nftl_config *cfg, nftl_block_t block) {
    uint8_t spare_area_on_page[SPARE_AREA_BYTES] = {0xff};

    assert(block < cfg->block_count);

    if (nftl_bd_isbad(cfg, block)) return -1; // bad block

    // simulate bad block, near threshold point but give some random
    _op_cnt[block]++;
    if (_op_cnt[block] > (BAD_THRESHOLD + rand() % BAD_THRESHOLD_RANDOM)) {
        printf("%s Block %d failed at %d-th operation.\n", __func__, block, _op_cnt[block]);
        return -1;
    }

    //nftl_bd_prog(cfg, block, 0, _erased_block, cfg->block_size);
    nftl_bd_prog_with_spare(cfg, block, 0, _erased_block, cfg->block_size,
                            spare_area_on_page, sizeof(spare_area_on_page));
    return 0;
}

int nftl_bd_sync(const struct nftl_config *cfg) {
    int fd = (intptr_t)cfg->context;

    int err = fsync(fd);
    if (err) {
        printf("%s %d Failed to fsync nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    return 0;
}

int nftl_bd_markbad(const struct nftl_config *cfg, nftl_block_t block)
{
    uint8_t flag;
    int fd = (intptr_t)cfg->context;

    // check if write is valid
    assert(block < cfg->block_count);

    printf("Marking block %d bad now.\n", block);

    flag = *(uint8_t *)(file_buffer + MAIN_AREA_SIZE + block * (SPARE_AREA_BYTES * NFTL_PAGE_PER_BLOCK_NUMS) + 9);
    if (flag == 0xee) return 0; /* already marked a bad */

    /* bad block flag 0xee is stored on the sixth byte */
    *(uint8_t *)(file_buffer + MAIN_AREA_SIZE + block * (SPARE_AREA_BYTES * NFTL_PAGE_PER_BLOCK_NUMS) + 9) = 0xee;

    // go to block
    off_t err = lseek(fd, 0, SEEK_SET);
    if (err < 0) {
        printf("%s %d Failed to ioctl nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    // write block
    ssize_t res = write(fd, file_buffer, sizeof(file_buffer));
    if (res < 0) {
        printf("%s %d Failed to write nftl device file, errno: %d\n", __func__, __LINE__, errno);
        return -errno;
    }

    return 0;
}

bool nftl_bd_isbad(const struct nftl_config *cfg, nftl_block_t block)
{
    uint8_t flag;
    assert(block < cfg->block_count);
    flag = *(uint8_t *)(file_buffer + MAIN_AREA_SIZE + block * (SPARE_AREA_BYTES * NFTL_PAGE_PER_BLOCK_NUMS) + 9);
    return flag == 0xee ? true : false;
}
