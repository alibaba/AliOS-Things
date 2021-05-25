/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_LITTLEFS_H
#define FS_LITTLEFS_H

#ifdef __cplusplus
extern "C" {
#endif

struct littlefs_cfg_param {
    /**
     * @brief Minimum size of a block read. All read operations will be a
     *        multiple of this value.
     */
    unsigned int read_size;

    /**
     * @brief Minimum size of a block program. All program operations will be a
     *         multiple of this value.
     */
     unsigned int prog_size;

    /**
     * @brief Size of an erasable block. This does not impact ram consumption and
     *        may be larger than the physical erase size. However, non-inlined files
     *        take up at minimum one block. Must be a multiple of the read
     *        and program sizes.
     */
    unsigned int block_size;

    /**
     * @brief Number of erasable blocks on the device.
     */
    unsigned int block_count;

    /**
     * @brief Number of erase cycles before littlefs evicts metadata logs and moves
     *        the metadata to another block. Suggested values are in the
     *        range 100-1000, with large values having better performance at the cost
     *        of less consistent wear distribution.
     *
     * @note Set to -1 to disable block-level wear-leveling.
     */
    int block_cycles;

    /**
     * @brief Size of block caches. Each cache buffers a portion of a block in RAM.
     *        The littlefs needs a read cache, a program cache, and one additional
     *        cache per file. Larger caches can improve performance by storing more
     *        data and reducing the number of disk accesses. Must be a multiple of
     *        the read and program sizes, and a factor of the block size.
     */
    unsigned int cache_size;
    /**
     * @brief Size of the lookahead buffer in bytes. A larger lookahead buffer
     *        increases the number of blocks found during an allocation pass. The
     *        lookahead buffer is stored as a compact bitmap, so each byte of RAM
     *        can track 8 blocks. Must be a multiple of 8.
     */
    unsigned int lookahead_size;
};

/**
 * @brief Fetch littlefs config params
 *
 * @param @param[in] cfg Pointer to littlefs_cfg struct
 *
 * @return On success, return 0, else return -1
 */
int32_t littlefs_fetch_cfg_param(struct littlefs_cfg_param *cfg_param);

/**
 * @brief Mount littlefs file system and register VFS driver for it
 *
 * @return On success, return 0, else return negative error code
 */
int32_t littlefs_register(void);

/**
 * @brief Unmount littlefs file system and unregister VFS driver for it
 *
 * @return On success, return 0, else return negative error code
 */
int32_t littlefs_unregister(void);

/**
 * @brief Used to format a specified partition.
 * This API is expected to be used after littlefs initialization.
 *
 * @param @param[in] partition The partition name, e.g. "/data" or "/system"
 *
 * @return On success, return 0, else return negative value
 */
int littlefs_format(const char *partition);

/**
 * @brief Used to format a specified partition.
 * This API is expected to be used before nftl/littlefs initialization.
 *
 * @param @param[in] partition The partition name, e.g. "/data" or "/system"
 *
 * @return On success, return 0, else return negative value
 */
int littlefs_format2(const char *partition);

#ifdef AOS_COMP_NFTL
/**
 * @brief Used to cleanup a specified partition, e.g. erase free blocks.
 *
 * @param @param[in] partition The partition name, e.g. "/data" or "/system"
 *
 * @return On success, return 0, else return negative value
 */
int littlefs_clean_partition(const char *partition);
#endif /* AOS_COMP_NFTL */

/**
 * @brief List the mounted littlefs filesystem.
 *
 * @return On success, return 0, else return negative error code
 */
int lfs_list();

#ifdef __cplusplus
}
#endif

#endif

