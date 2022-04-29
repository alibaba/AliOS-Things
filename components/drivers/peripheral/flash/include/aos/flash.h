/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_FLASH_H
#define AOS_FLASH_H

#ifdef AOS_KERNEL_BUILD
#include <aos/device.h>
#else
#include <stdint.h>
#endif

/**
 * @defgroup flash_api Flash
 * @ingroup driver_api
 * @brief AOS API for flash.
 * @{
 */

#define AOS_FLASH_F_TYPE_MASK                   ((uint32_t)0xF << 0)
#define AOS_FLASH_F_TYPE_NOR                    ((uint32_t)0x0 << 0)
#define AOS_FLASH_F_TYPE_NAND                   ((uint32_t)0x1 << 0)
#define AOS_FLASH_F_ECC_STRENGTH_MASK           ((uint32_t)0xFFFF << 16)
#define AOS_FLASH_F_ECC_STRENGTH(x)             (((uint32_t)(x) & 0xFFFF) << 16)

#define aos_flash_flags_to_ecc_strength(f)      (((f) & AOS_FLASH_F_ECC_STRENGTH_MASK) >> 16)
#define AOS_FLASH_ECC_ERROR                     (aos_flash_flags_to_ecc_strength(AOS_FLASH_F_ECC_STRENGTH_MASK) + 1)

#ifdef AOS_KERNEL_BUILD

typedef aos_dev_ref_t aos_flash_ref_t;

typedef struct {
    uint32_t flags;
    uint64_t block_count;
    size_t pages_per_block;
    size_t page_data_size;
    size_t page_spare_size;
} aos_flash_info_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a flash device.
 * @param   ref Flash ref to operate.
 * @param   id  Flash device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_get(aos_flash_ref_t *ref, uint32_t id);
/**
 * @brief   Release a flash device.
 * @param   ref Flash ref to operate.
 * @return  None.
 */
void aos_flash_put(aos_flash_ref_t *ref);
/**
 * @brief   Get information about a flash device.
 * @param   ref     Flash ref to operate.
 * @param   info    Information structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_get_info(aos_flash_ref_t *ref, aos_flash_info_t *info);
/**
 * @brief   Read data from a flash device.
 * @param   ref     Flash ref to operate.
 * @param   offset  Offset from the start of the flash device.
 * @param   buf     Data buffer.
 * @param   count   Number of bytes to read.
 * @return  >= 0 and <= ECC strength: bit-flips corrected; AOS_FLASH_ECC_ERROR: on ECC error; < 0: on failure.
 */
aos_status_t aos_flash_read(aos_flash_ref_t *ref, uint64_t offset, void *buf, size_t count);
/**
 * @brief   Write data to a flash device.
 * @param   ref     Flash ref to operate.
 * @param   offset  Offset from the start of the flash device.
 * @param   buf     Data buffer.
 * @param   count   Number of bytes to write.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_write(aos_flash_ref_t *ref, uint64_t offset, const void *buf, size_t count);
/**
 * @brief   Erase an area on a flash device.
 * @param   ref     Flash ref to operate.
 * @param   offset  Offset from the start of the flash device.
 * @param   size    The size of the area to erase in bytes.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_erase(aos_flash_ref_t *ref, uint64_t offset, size_t size);
/**
 * @brief   Read data from a page.
 * @param   ref             Flash ref to operate.
 * @param   page            Page to operate.
 * @param   data_offset     Offset from the start of the in-band area.
 * @param   data_buf        In-band buffer.
 * @param   data_count      Number of bytes to read from the in-band area.
 * @param   spare_offset    Offset from the start of the out-of-band area.
 * @param   spare_buf       Out-of-band buffer.
 * @param   spare_count     Number of bytes to read from the out-of-band area.
 * @return  >= 0 and <= ECC strength: bit-flips corrected; AOS_FLASH_ECC_ERROR: on ECC error; < 0: on failure.
 */
aos_status_t aos_flash_read_page(aos_flash_ref_t *ref, uint64_t page,
                                 size_t data_offset, void *data_buf, size_t data_count,
                                 size_t spare_offset, void *spare_buf, size_t spare_count);
/**
 * @brief   Write data to a page.
 * @param   ref             Flash ref to operate.
 * @param   page            Page to operate.
 * @param   data_offset     Offset from the start of the in-band area.
 * @param   data_buf        In-band buffer.
 * @param   data_count      Number of bytes to write to the in-band area.
 * @param   spare_offset    Offset from the start of the out-of-band area.
 * @param   spare_buf       Out-of-band buffer.
 * @param   spare_count     Number of bytes to write to the out-of-band area.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_write_page(aos_flash_ref_t *ref, uint64_t page,
                                  size_t data_offset, const void *data_buf, size_t data_count,
                                  size_t spare_offset, const void *spare_buf, size_t spare_count);
/**
 * @brief   Erase a block.
 * @param   ref     Flash ref to operate.
 * @param   block   Block to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_erase_block(aos_flash_ref_t *ref, uint64_t block);
/**
 * @brief   Check the bad block mark.
 * @param   ref     Flash ref to operate.
 * @param   block   Block to operate.
 * @return  0: good block; 1: bad block; < 0: on failure.
 */
aos_status_t aos_flash_is_bad_block(aos_flash_ref_t *ref, uint64_t block);
/**
 * @brief   Mark a bad block.
 * @param   ref     Flash ref to operate.
 * @param   block   Block to operate.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_flash_mark_bad_block(aos_flash_ref_t *ref, uint64_t block);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_BUILD */

/** @} */

#endif /* AOS_FLASH_H */
