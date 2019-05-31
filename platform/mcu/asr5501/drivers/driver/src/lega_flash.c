#include <stdio.h>
#include <string.h>
#include "lega_cm4.h"
#include "lega_flash_alg.h"
#include "lega_flash.h"

extern const lega_logic_partition_t lega_partitions[];

#define ROUND_DOWN(x, align) ((unsigned long)(x) & ~((unsigned long)align - 1))

/**
 * lega flash init
 *
 * @note this function must be called before flash erase/write operation
 *       also, this function must not be interrupted, it should be called
 *       with interrupt disabled
 * @param[in]  none
 *
 * @return     HAL_logi_partition struct
 */
int32_t lega_flash_init(void)
{
    lega_flash_alg_cache_flush();
    return lega_flash_alg_init();
}
/**
 * Get the infomation of the specified flash area
 *
 * @param[in]  in_partition  The target flash logical partition
 *
 * @return     HAL_logi_partition struct
 */
lega_logic_partition_t *lega_flash_get_info(lega_partition_t in_partition)
{
    lega_logic_partition_t *logic_partition;

    logic_partition = (lega_logic_partition_t *)&lega_partitions[ in_partition ];

    return logic_partition;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *        this function must not be interrupted, it should be called with
 *        interrupt disabled
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_erase(lega_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    lega_logic_partition_t *partition_info;

    partition_info = lega_flash_get_info( in_partition );
    if(size + off_set > partition_info->partition_length)
    {
        return -1;
    }

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SEC_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SEC_SIZE)
    {
        lega_flash_alg_cache_flush();
        ret = lega_flash_alg_erasesector(addr);
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}


/**
 * Write data to an area on a flash logical partition without erase
 *
 * @note this function must not be interrupted, it should be called with
 *       interrupt disabled
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_write(lega_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    uint32_t start_addr, end_addr;
    lega_logic_partition_t *partition_info;
    uint8_t *p_buf = (uint8_t *)(in_buf);
    uint32_t prg_size;
    uint32_t left_buf_len = in_buf_len;

    partition_info = lega_flash_get_info( in_partition );
    if(off_set == NULL || in_buf == NULL || ((*off_set + in_buf_len) > partition_info->partition_length))
    {
        // ptr and size over range check
        return -1;
    }
    start_addr = partition_info->partition_start_addr + *off_set;
    end_addr = start_addr + left_buf_len;

    while(left_buf_len > 0)
    {
        if((end_addr / SPI_FLASH_PAGE_SIZE) > (start_addr / SPI_FLASH_PAGE_SIZE))
        {
            prg_size = SPI_FLASH_PAGE_SIZE - (start_addr % SPI_FLASH_PAGE_SIZE);
        }
        else
        {
            prg_size = left_buf_len;
        }
        lega_flash_alg_cache_flush();
        lega_flash_alg_programpage(start_addr, prg_size, p_buf);
        p_buf += prg_size;
        start_addr += prg_size;
        left_buf_len -= prg_size;
    }

    *off_set += in_buf_len;
    return 0;
}

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @note this function must not be interrupted, it should be called with
 *       interrupt disabled
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_erase_write(lega_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    ret = lega_flash_erase(in_partition, *off_set, in_buf_len);
    if (ret != 0)
    {
        return ret;
    }
    return lega_flash_write(in_partition, off_set, in_buf, in_buf_len);
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_read(lega_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    lega_logic_partition_t *partition_info;

    partition_info = lega_flash_get_info( in_partition );

    if(off_set == NULL || out_buf == NULL || *off_set + in_buf_len > partition_info->partition_length)
    {
        return -1;
    }
    start_addr = partition_info->partition_start_addr + *off_set;
    memcpy(out_buf,(void *)(start_addr),in_buf_len);
    *off_set += in_buf_len;

    return ret;
}


/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_enable_secure(lega_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t lega_flash_dis_secure(lega_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

