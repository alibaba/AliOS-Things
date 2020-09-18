#ifndef _NAND_FLASH_HAL_
#define _NAND_FLASH_HAL_

#include <aos/hal/flash.h>

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
