#ifndef __NFTL_H
#define __NFTL_H

#include "nftl_conf.h"
#include "nftl_util.h"

/* FTL error codes */
typedef enum {
    NFTL_ERR_OK,
    NFTL_ERR_INVALID_ARGUMENT,
    NFTL_ERR_NAND_BAD,
    NFTL_ERR_NAND_IO,
    NFTL_ERR_NAND_NO_PAGE,
    NFTL_ERR_NAND_ERASED_PENDING_WRITE,
    NFTL_ERR_NAND_UNKNOWN,
} nftl_err_t;

/* FTL logical parition index, support 3 for now, flexible to add more later */
typedef enum {
    NFTL_PARTITION0,
    NFTL_PARTITION1,
    NFTL_PARTITION2,
    NFTL_PARTITION_MAX
} nftl_partition_t;

/* FTL logical partition table information */
typedef struct {
    const char  *desc; /* description */
    uint32_t     blkoff; /* block start offset */
    uint32_t     blkcnt; /* block count */
} nftl_partition_info_t;


#define NFTL_BLKS_BITMAP_LEN     ((NFTL_WORK_BLOCK_NUMS + 7) / 8)

#define NFTL_PAGES_PER_BLK       64
#define NFTL_PAGE_BITMAP_LEN     ((NFTL_PAGES_PER_BLK + 7) / 8)

#define NFTL_TAG_PAGE_IN_BLOCK   (NFTL_PAGES_PER_BLK - 1)

#define NFTL_INVALID_BLOCK       0xFFFFFFFF

#define NFTL_SPARE_AREA_LEN      16

typedef uint32_t nftl_block_t;
typedef uint32_t nftl_offset_t;
typedef uint32_t nftl_size_t;

typedef struct {
    uint8_t                 page_bitmap[NFTL_PAGE_BITMAP_LEN];
} nftl_phy_block_info_t;

typedef struct {
    uint32_t                page_id;
    uint32_t                reversion;
} nftl_logic_info_t;

typedef struct {
    uint32_t                logic_block;
    uint32_t                reversion;
} nftl_block_tag;

typedef struct {
    uint8_t                 page_buff[NFTL_PAGE_SIZE];
    uint8_t                 spare_buff[NFTL_SPARE_AREA_LEN];
    uint32_t                 phy_block_nums;
    nftl_phy_block_info_t  *phy_blocks;
    uint32_t                logic_block_nums;
    nftl_logic_info_t      *logic_blocks;
    uint32_t                allocating_blk;
    uint32_t                allocating_page;
    nftl_block_tag          allocating_blk_tags[NFTL_TAG_PAGE_IN_BLOCK];
    hal_partition_t         partition;
    nftl_lock_t             par_lock;
    uint8_t                 initialized;
    uint32_t                bad_block_nums;
    uint32_t                gc_times;
} nftl_block_mgr_t;

void nftl_init(void);

void nftl_deinit(void);

int nftl_flash_read(nftl_partition_t p, nftl_block_t blk, nftl_offset_t off,
                    void *outbuf, nftl_size_t len);

int nftl_flash_write(nftl_partition_t p, nftl_block_t blk, nftl_size_t off,
                     const void *inbuf, nftl_size_t len);

int nftl_flash_erase(nftl_partition_t p, nftl_block_t blk);

int nftl_flash_sync(nftl_partition_t p);

void nftl_dump(nftl_block_mgr_t *mgr);

#ifdef NFTL_GC_NOTIFY
    nftl_err_t nftl_flash_notify_gabage(nftl_partition_t p, nftl_block_t blk);
#endif /* NFTL_GC_NOTIFY */

#endif

