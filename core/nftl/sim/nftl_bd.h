/*
 * Linux user-space block device wrapper
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NFTL_BD_H_
#define _NFTL_BD_H_

#include <stdint.h>
#include <stdbool.h>

#define SPARE_AREA_BYTES 16 /* bytes */

typedef uint32_t nftl_size_t;
typedef uint32_t nftl_off_t;
typedef uint32_t nftl_block_t;

// Configuration provided during initialization of the nand nftl block device
struct nftl_config {
    // Opaque user provided context that can be used to pass
    // information to the block device operations
    void *context;

    // Read a region in a block. Negative error codes are propogated
    // to the user.
    int (*read)(const struct nftl_config *c, nftl_block_t block,
                nftl_off_t off, void *buffer, nftl_size_t size);

    // Program a region in a block. The block must have previously
    // been erased. Negative error codes are propogated to the user.
    // May return LFS_ERR_CORRUPT if the block should be considered bad.
    int (*prog)(const struct nftl_config *c, nftl_block_t block,
                nftl_off_t off, const void *buffer, nftl_size_t size);

    // Erase a block. A block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propogated to the user.
    // May return LFS_ERR_CORRUPT if the block should be considered bad.
    int (*erase)(const struct nftl_config *c, nftl_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propogated to the user.
    int (*sync)(const struct nftl_config *c);

    // Minimum size of a block read. All read operations will be a
    // multiple of this value.
    nftl_size_t read_size;

    // Minimum size of a block program. All program operations will be a
    // multiple of this value.
    nftl_size_t prog_size;

    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, non-inlined files
    // take up at minimum one block. Must be a multiple of the read
    // and program sizes.
    nftl_size_t block_size;

    // Number of erasable blocks on the device.
    nftl_size_t block_count;
};

// Create a block device with path to dev block device
int nftl_bd_create(struct nftl_config *cfg, const char *path);

// Clean up memory associated with emu block device
void nftl_bd_destroy(const struct nftl_config *cfg);

// Read a block
int nftl_bd_read(const struct nftl_config *cfg, nftl_block_t block,
               nftl_off_t off, void *buffer, nftl_size_t size);

int nftl_bd_read_with_spare(const struct nftl_config *cfg, nftl_block_t block,
                            nftl_off_t off, void *buffer, nftl_size_t size,
                            void *spare_buffer, nftl_size_t spare_size);

// Program a block
//
// The block must have previously been erased.
int nftl_bd_prog(const struct nftl_config *cfg, nftl_block_t block,
               nftl_off_t off, const void *buffer, nftl_size_t size);

int nftl_bd_prog_with_spare(const struct nftl_config *cfg, nftl_block_t block,
                            nftl_off_t off, const void *buffer, nftl_size_t size,
                            const void *spare_buffer, nftl_size_t spare_size);

// Erase a block
//
// A block must be erased before being programmed. The
// state of an erased block is undefined.
int nftl_bd_erase(const struct nftl_config *cfg, nftl_block_t block);

// Sync the block device
int nftl_bd_sync(const struct nftl_config *cfg);

bool nftl_bd_isbad(const struct nftl_config *cfg, nftl_block_t block);
int nftl_bd_markbad(const struct nftl_config *cfg, nftl_block_t block);
#endif /* _NFTL_BD_H_ */
