#ifndef _NAND_FLASH_HAL_
#define _NAND_FLASH_HAL_

#include <hal/flash.h>

typedef struct {
    /* manufacturer ID */
    uint8_t mid;

    /* Device ID */
    uint8_t did;

    /* Organization ID */
    uint8_t oid;

    /* internal chip number:
     *   0b00 - 1,
     *   0b01 - 2,
     *   0b10 - 4,
     *   0b11 - 8
     */
    uint8_t internal_chip_number:2;

    /* cell type:
     *   0b00 : 2-level cell,
     *   0b01 : 4-level cell,
     *   0b10 : 8-level cell,
     *   0b11 : 16-level cell,
     */
    uint8_t cell_type:2;

    /* Number of simutaneously programmed pages:
     *   0b00 - 1
     *   0b01 - 2
     *   0b10 - 4
     *   0b11 - 8
     */
    uint8_t spages:2;

    /* Interleave program between multiple chips:
     *   0b0 - not supported
     *   0b1 - supported
     */
    uint8_t interleave_prog:1;

    /* Cache program:
     *   0b0 - not supported
     *   0b1 - supported
     */
    uint8_t cache_prog:1;

    /* Page size (witout spare area):
     *   0b00 - 1kB
     *   0b01 - 2kB
     *   0b10 - 4kB
     *   0b11 - 8kB
     */
    uint8_t page_size:2;

    /* Block Size (without spare area):
     *   0b00 - 64kB
     *   0b01 - 128kB
     *   0b10 - 256kB
     *   0b11 - 512kB
     */
    uint8_t block_size:2;

    /* Spare ares size:
     *   0b0000 - 8 Bytes
     *   0b0001 - 16 Bytes
     *   0b0010 - 32 Bytes
     *   0b0011 - 64 Bytes
     *   0b0100 - 128 Bytes
     *   0b0101 - 256 Bytes
     *   0b0110 - 512 Bytes
     *   0b0111 - 1024 Bytes
     *   other - reserved
     */
    uint8_t spare_size:4;

    /* Plane number
     *   0b00 - 1
     *   0b01 - 2
     *   0b10 - 4
     *   0b11 - 8
     */
    uint8_t plane_number:2;

    /* Plane size (without spare area):
     *   0b000 - 64Mb
     *   0b001 - 128Mb
     *   0b010 - 256Mb
     *   0b011 - 512Mb
     *   0b000 - 1Gb
     *   0b000 - 2Gb
     *   0b000 - 4Gb
     *   0b000 - reserved
     */
    uint8_t plane_size:3;

    uint8_t reserved:3;
} nand_flash_id_t;

/**
 * Read ID of NAND Flash.
 *
 * @param[out]  id  The NAND Flash ID information structure.
 *
 * @return  HAL_FLASH_ERR_OK : Success, otherwise failure
 */
hal_flash_err_t hal_flash_read_id(nand_flash_id_t *id);

/**
 * Check offset in partition at partition id is a bad block or not for nand
 *
 * @param[in]  in_partition    The target flash logical partition which should be check
 * @param[in]  off_set         Point to the address in partition
 *
 * @return  0 : Good block, 1: Bad block, EIO : If an error occurred with any step
 */
int hal_flash_isbad (hal_partition_t in_partition, uint32_t off_set);

/**
 * Markbad at offset in partition at partition id for nand
 *
 * @param[in]  in_partition    The target flash logical partition which should be check
 * @param[in]  off_set         Point to the address in partition
 *
 * @return  0 : On success, others : On fail
 */
int hal_flash_markbad(hal_partition_t in_partition, uint32_t off_set);

/**
 * Get the page size for nand
 *
 * @return page size
 */
uint32_t hal_flash_get_pgsize();

/**
 * Get the block size for nand
 *
 * @return block size
 */
uint32_t hal_flash_get_blksize();

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  data_buf        point to the data buffer that will be written to flash
 * @param[in]  data_buf_len    The length of the data buffer
 * @param[in]  spare_buf       point to the spare data buffer that will be written to flash
 * @param[in]  spare_buf_len   The length of the spare buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write_with_spare(hal_partition_t in_partition, uint32_t *off_set,
                                   const void *data_buf, uint32_t data_buf_len,
                                   const void *spare_buf, uint32_t spare_buf_len);

/**
 * Read data (with spare) from an area on a Flash to data buffer in RAM,
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  data_buf        Point to the data buffer that stores the data read from flash
 * @param[in]  data_buf_len    The length of the data buffer
 * @param[in]  spare_buf       Point to the spare data buffer that stores the data read from flash
 * @param[in]  spare_buf_len   The length of the spare buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read_with_spare(hal_partition_t in_partition, uint32_t *off_set,
                                  void *data_buf, uint32_t data_buf_len,
                                  void *spare_buf, uint32_t spare_buf_len);

#endif /* _NAND_FLASH_HAL_ */
