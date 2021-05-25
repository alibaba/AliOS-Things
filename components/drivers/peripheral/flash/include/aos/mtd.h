/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __AOS_MTD_H
#define __AOS_MTD_H

#include <stddef.h>
#include <stdint.h>
#include <aos/kernel.h>
#include "device/aos_device.h"

#define MTD_TYPE_NOR    1
#define MTD_TYPE_NAND   2

#define MTD_DEVICE_NAME_PREFIX "mtdblock"
#define MTD_DEVICE_NAME_FORMAT MTD_DEVICE_NAME_PREFIX"%u"
#define MTD_PARTITION_NAME_MAX 16
#define MTD_DEVICE_NAME_MAX 16

#define IOC_MTD_BASE   'M'
#define IOC_MTD_GET_SIZE      (IOC_MTD_BASE + 0x1)
#define IOC_MTD_GET_OFFSET    (IOC_MTD_BASE + 0x2)
#define IOC_MTD_ERASE         (IOC_MTD_BASE + 0x3)

/* standard partition names */
#define MTD_PART_NAME_BOOTLOADER "BOOT"
#define MTD_PART_NAME_BOOTLOADER_SEC "BOOTSEC"
#define MTD_PART_NAME_2ND_BOOTLOADER "BOOT2A"
#define MTD_PART_NAME_2ND_BOOTLOADER2 "BOOT2B"
#define MTD_PART_NAME_ENV1 "ENV"
#define MTD_PART_NAME_ENV2 "ENV2"
#define MTD_PART_NAME_DTB "DTB"
#define MTD_PART_NAME_KERNEL "KERNEL"
#define MTD_PART_NAME_KERNEL2 "KERNEL2"
#define MTD_PART_NAME_ROMFS "ROMFS"
#define MTD_PART_NAME_ROMFS2 "ROMFS2"
#define MTD_PART_NAME_LITTLEFS "LFS"
#define MTD_PART_NAME_LITTLEFS2 "LFS2"
#define MTD_PART_NAME_APP "APP"
#define MTD_PART_NAME_FACTORY "FACTORY"
#define MTD_PART_NAME_FACTORY2 "FACTORY2"
#define MTD_PART_NAME_KV "KV"
#define MTD_PART_NAME_UNKNOWN "UNKNOWN"
/* subject to add more hereafter ... */

 /**
  * MTD operation modes
  *
  * @MTD_OPM_PLACE_OOB:    OOB data are placed at the given offset (default)
  * @MTD_OPM_AUTO_OOB:    OOB data are automatically placed at the free areas
  * @MTD_OPM_RAW:        data are transferred as-is, with no error correction;
  */
enum mtd_opm
{
    MTD_OPM_PLACE_OOB = 0,
    MTD_OPM_AUTO_OOB = 1,
    MTD_OPM_RAW = 2,
};

struct mtd_erase_info {
    unsigned long offset;
    unsigned long length;
};

struct mtd_block_info {
    unsigned long block_size;
    unsigned long block_start;
    unsigned long block_end;
};


struct mtd_oob_region
{
    uint8_t offset;
    uint8_t length;
};

typedef struct mtd_info
{
    struct aos_device parent;
    const struct mtd_ops *ops;

    uint16_t oob_size;
    uint16_t sector_size;   /* Minimal writable flash unit size */
    uint32_t block_size:28; /* Erase size for the device */
    uint32_t type:4;

    size_t size;    /* Total size of the MTD */
    off_t offset;  /* At which this MTD starts, from the beginning of the MEMORY */
    struct mtd_info *master;

    void *priv;
} aos_mtd_t;

struct mtd_io_desc
{
    uint8_t mode;      /* operation mode(enum mtd_opm) */
    uint8_t ooblen;    /* number of oob bytes to write/read */
    uint8_t oobretlen; /* number of oob bytes written/read */
    uint8_t ooboffs;   /* offset in the oob area  */
    uint8_t *oobbuf;

    size_t  datlen;    /* number of data bytes to write/read */
    size_t  datretlen; /* number of data bytes written/read */
    uint8_t *datbuf;   /* if NULL only oob are read/written */
};

struct mtd_ops
{
    int(*erase)(aos_mtd_t *mtd, off_t addr, size_t len);    /* return 0 if success */
    int(*read) (aos_mtd_t *mtd, off_t from, struct mtd_io_desc *ops); /* return 0 if success */
    int(*write) (aos_mtd_t *mtd, off_t to, struct mtd_io_desc *ops);  /* return 0 if success */
    int(*isbad) (aos_mtd_t *mtd, uint32_t block);    /* return 1 if bad, 0 not bad */
    int(*markbad) (aos_mtd_t *mtd, uint32_t block);  /* return 0 if success */
};

struct mtd_part
{
    const char *name;           /* platform specific name of the MTD partition provided, optionally to be the same as name_std */
    const char *name_std;       /* standard name of the MTD partition define in alios */
    off_t offset;               /* start addr of partion */
    size_t size;                /* size of partion */
};

struct mtd_part_info
{
    off_t offset;                               /* start addr of partion */
    size_t size;                                /* size of partion */
    char part_name[MTD_PARTITION_NAME_MAX];     /* original name of the MTD partition, usually provided by platform */
    char part_name_std[MTD_PARTITION_NAME_MAX]; /* standard name of the MTD partition, defined by alios */
    char dev_name[MTD_DEVICE_NAME_MAX];         /* device name of the MTD partition */
};

/**
 * Open a mtd device
 *
 * @param[in]   name      partition name
 *
 * @return      A mtd device handle, NULL: error
 */
aos_mtd_t* aos_mtd_open(const char *name);

/**
 * Close a mtd device
 *
 * @param[in]   mtd       A mtd device handle
 *
 * @return      0:success, <0: error
 */
aos_status_t aos_mtd_close(aos_mtd_t *mtd);

/**
 * Read data from mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   offset    The offset address of mtd device which read from
 * @param[out]  buf       The buffer to store data read from mtd device
 * @param[in]   size      The size of request to read
 *
 * @return      >0：The size of data that really read; 0: end of device; <0: error
 */
ssize_t aos_mtd_read(aos_mtd_t *mtd, off_t offset, void *buf, size_t size);

/**
 * Read out-of-band data from mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   offset    The offset address of mtd device which read from
 * @param[in]   info      The description information of mtd IO
 *
 * @return      >0：The size of out-of-band data that really read; 0: end of device; <0: error
 */
ssize_t aos_mtd_oob_read(aos_mtd_t *mtd, off_t offset, struct mtd_io_desc *info);

/**
 * Write data to mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   offset    The offset address of mtd device which write to
 * @param[in]   buf       The buffer to store data to be written
 * @param[in]   size      The size of request to write

 * @return      >0：The size of data that really be written; <0: error
 */
ssize_t aos_mtd_write(aos_mtd_t *mtd, off_t offset, const void *buf, size_t size);

/**
 * Write out-of-band data to mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   offset    The offset address of mtd device which write to
 * @param[in]   info      The description information of mtd IO
 *
 * @return      >0：The size of out-of-band data that really written; <0: error
 */
ssize_t aos_mtd_oob_write(aos_mtd_t *mtd, off_t offset, struct mtd_io_desc *info);

/**
 * Erase a region of mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   offset    The offset address of mtd device to be erased
 * @param[in]   size      The size of request to be erased
 *
 * @return      0: success, <0: error
 */
aos_status_t aos_mtd_erase(aos_mtd_t *mtd, off_t offset, size_t size);

/**
 * Erase a block of mtd device
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   block     The block index to be erased
 *
 * @return      0: success, <0: error
 */
aos_status_t aos_mtd_block_erase(aos_mtd_t *mtd, uint32_t block);

/**
 * Mark a block of mtd device as bad
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   block     The block index to be marked
 *
 * @return      0: success, <0: error
 */
aos_status_t aos_mtd_block_mark_bad(aos_mtd_t *mtd, uint32_t block);

/**
 * Check if a block of mtd device is bad
 *
 * @param[in]   mtd       A mtd device handle
 * @param[in]   block     The block index to be checked
 *
 * @return      0: success, <0: error
 */
aos_status_t aos_mtd_block_is_bad(aos_mtd_t *mtd, uint32_t block);


/* get mtd partition information, caller to free the info memory */
int aos_mtd_part_info_get(struct mtd_part_info **info, int *cnt);

/* register mtd paritions */
int aos_mtd_register(aos_mtd_t *master, const struct mtd_part *parts, int np);

#endif /*__AOS_MTD_H*/
