/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_FLASHPART_H
#define AOS_FLASHPART_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif
#include <aos/flash.h>

/**
 * @defgroup flashpart_api Flash Partition
 * @ingroup driver_api
 * @brief AOS API for flash partition.
 * @{
 */

#if (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD)

typedef struct {
    uint32_t flash_id;
    uint32_t flash_flags;
    uint64_t flash_block_count;
    uint64_t block_start;
    uint64_t block_count;
    size_t pages_per_block;
    size_t page_data_size;
    size_t page_spare_size;
} aos_mtd_info_t;

typedef struct {
    uint64_t offset;
    size_t size;
} aos_mtd_erase_args_t;

typedef struct {
    uint64_t page;
    size_t data_offset;
    void *data_buf;
    size_t data_count;
    size_t spare_offset;
    void *spare_buf;
    size_t spare_count;
} aos_mtd_read_page_args_t;

typedef struct {
    uint64_t page;
    size_t data_offset;
    const void *data_buf;
    size_t data_count;
    size_t spare_offset;
    const void *spare_buf;
    size_t spare_count;
} aos_mtd_write_page_args_t;

#define AOS_MTD_IOC_GET_INFO            0x4D01
#define AOS_MTD_IOC_ERASE               0x4D02
#define AOS_MTD_IOC_READ_PAGE           0x4D03
#define AOS_MTD_IOC_WRITE_PAGE          0x4D04
#define AOS_MTD_IOC_ERASE_BLOCK         0x4D05
#define AOS_MTD_IOC_IS_BAD_BLOCK        0x4D06
#define AOS_MTD_IOC_MARK_BAD_BLOCK      0x4D07

#endif /* (defined(AOS_KERNEL_BUILD) && defined(AOS_COMP_DEVFS)) || !defined(AOS_KERNEL_BUILD) */

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_flashpart_ref_t;

typedef struct {
    uint32_t flash_id;
    uint64_t block_start;
    uint64_t block_count;
} aos_flashpart_info_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a flash partition.
 * @param   ref Flash partition ref to operate.
 * @param   id  Flash partition ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_get(aos_flashpart_ref_t *ref, uint32_t id);
/**
 * @brief   Release a flash partition.
 * @param   ref Flash partition ref to operate.
 * @return  None.
 */
void aos_flashpart_put(aos_flashpart_ref_t *ref);
/**
 * @brief   Get information about a flash partition.
 * @param   ref         Flash partition ref to operate.
 * @param   info        Flash partition information structure.
 * @param   flash_info  Flash device information structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_get_info(aos_flashpart_ref_t *ref, aos_flashpart_info_t *info, aos_flash_info_t *flash_info);
/**
 * @brief   Read data from a flash partition.
 * @param   ref     Flash partition ref to operate.
 * @param   offset  Offset from the start of the flash partition.
 * @param   buf     Data buffer.
 * @param   count   Number of bytes to read.
 * @return  >= 0 and <= ECC strength: bit-flips corrected; AOS_FLASH_ECC_ERROR: on ECC error; < 0: on failure.
 */
aos_status_t aos_flashpart_read(aos_flashpart_ref_t *ref, uint64_t offset, void *buf, size_t count);
/**
 * @brief   Write data to a flash partition.
 * @param   ref     Flash partition ref to operate.
 * @param   offset  Offset from the start of the flash partition.
 * @param   buf     Data buffer.
 * @param   count   Number of bytes to write.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_write(aos_flashpart_ref_t *ref, uint64_t offset, const void *buf, size_t count);
/**
 * @brief   Erase an area on a flash partition.
 * @param   ref     Flash partition ref to operate.
 * @param   offset  Offset from the start of the flash partition.
 * @param   size    The size of the area to erase in bytes.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_erase(aos_flashpart_ref_t *ref, uint64_t offset, size_t size);
/**
 * @brief   Read data from a page.
 * @param   ref             Flash partition ref to operate.
 * @param   page            Page to operate.
 * @param   data_offset     Offset from the start of the in-band area.
 * @param   data_buf        In-band buffer.
 * @param   data_count      Number of bytes to read from the in-band area.
 * @param   spare_offset    Offset from the start of the out-of-band area.
 * @param   spare_buf       Out-of-band buffer.
 * @param   spare_count     Number of bytes to read from the out-of-band area.
 * @return  >= 0 and <= ECC strength: bit-flips corrected; AOS_FLASH_ECC_ERROR: on ECC error; < 0: on failure.
 */
aos_status_t aos_flashpart_read_page(aos_flashpart_ref_t *ref, uint64_t page,
                                     size_t data_offset, void *data_buf, size_t data_count,
                                     size_t spare_offset, void *spare_buf, size_t spare_count);
/**
 * @brief   Write data to a page.
 * @param   ref             Flash partition ref to operate.
 * @param   page            Page to operate.
 * @param   data_offset     Offset from the start of the in-band area.
 * @param   data_buf        In-band buffer.
 * @param   data_count      Number of bytes to write to the in-band area.
 * @param   spare_offset    Offset from the start of the out-of-band area.
 * @param   spare_buf       Out-of-band buffer.
 * @param   spare_count     Number of bytes to write to the out-of-band area.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_write_page(aos_flashpart_ref_t *ref, uint64_t page,
                                      size_t data_offset, const void *data_buf, size_t data_count,
                                      size_t spare_offset, const void *spare_buf, size_t spare_count);
/**
 * @brief   Erase a block.
 * @param   ref     Flash partition ref to operate.
 * @param   block   Block to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_erase_block(aos_flashpart_ref_t *ref, uint64_t block);
/**
 * @brief   Check the bad block mark.
 * @param   ref     Flash partition ref to operate.
 * @param   block   Block to operate.
 * @return  0: good block; 1: bad block; < 0: on failure.
 */
aos_status_t aos_flashpart_is_bad_block(aos_flashpart_ref_t *ref, uint64_t block);
/**
 * @brief   Mark a bad block.
 * @param   ref     Flash partition ref to operate.
 * @param   block   Block to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flashpart_mark_bad_block(aos_flashpart_ref_t *ref, uint64_t block);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_FLASHPART_H */
