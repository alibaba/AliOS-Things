/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "aos/hal/flash.h"
#include "fs/littlefs.h"

#include "nandx_util.h"
#include "nandx_core.h"
#include "nandx_os.h"
#include "bbt.h"

extern const hal_logic_partition_t hal_partitions[];
extern aos_mutex_t nandx_lock;
extern struct nandx_info nandxi;

extern int32_t littlefs_part_erase(hal_partition_t in_partition,
				   uint32_t off_set, uint32_t size);
extern int32_t littlefs_part_read(hal_partition_t in_partition,
				  uint32_t *off_set, void *out_buf,
				  uint32_t in_buf_len);
extern int32_t littlefs_part_write(hal_partition_t in_partition,
				   uint32_t *off_set, void *in_buf,
				   uint32_t in_buf_len);
extern int32_t littlefs_map_init(void);

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be
 *			     erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set,
			uint32_t size)
{
	int32_t ret;

	if (in_partition == HAL_PARTITION_LITTLEFS ||
		in_partition == HAL_PARTITION_LITTLEFS2)
		return littlefs_part_erase(in_partition, off_set, size);

	nandx_mutex_lock(&nandx_lock);
	if ((off_set + size) > hal_partitions[in_partition].partition_length) {
		pr_warn("%s, operation exceed partition: %d,"
			"partition_len: 0x%x, off: %d, len: %d\n",
			__func__, in_partition,
			hal_partitions[in_partition].partition_length,
			off_set, size);
		ret = -EIO;
		goto out;
	}

	off_set += hal_partitions[in_partition].partition_start_addr;
	ret = mtk_nand_erase_operation(off_set, size, false);

out:
	nandx_mutex_unlock(&nandx_lock);
	return ret;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should
 *			       be read
 * @param[in]  off_set         Point to the start address that the data is read,
 *			       and point to the last unread address after this
 *			       function is returned, so you can call this
 *			       function serval times without update this start
 *			       address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data
 *			       read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
	uint32_t offset, op_len = 0;
	int32_t ret;

	if (in_partition == HAL_PARTITION_LITTLEFS ||
		in_partition == HAL_PARTITION_LITTLEFS2)
		return littlefs_part_read(in_partition, off_set, out_buf,
					  in_buf_len);

	nandx_mutex_lock(&nandx_lock);
	if ((*off_set + in_buf_len) >
		hal_partitions[in_partition].partition_length) {
		pr_warn("%s, operation exceed partition: %d,"
			"partition_len: 0x%x, off: %d, len: %d\n",
			__func__, in_partition,
			hal_partitions[in_partition].partition_length,
			*off_set, in_buf_len);
		ret = -EIO;
		goto out;

	}

	offset = *off_set + hal_partitions[in_partition].partition_start_addr;
	ret = mtk_nand_rw_operation(out_buf, offset, in_buf_len, &op_len, true);
	*off_set += op_len;

out:
	nandx_mutex_unlock(&nandx_lock);
	return ret;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should
 *			       be read which should be written
 * @param[in]  off_set         Point to the start address that the data is
 *			       written to, and point to the last unwritten
 *			       address after this function is returned, so you
 *			       can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to
 *			       flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
	uint32_t offset, op_len = 0;
	int32_t ret;

	if (in_partition == HAL_PARTITION_LITTLEFS ||
		in_partition == HAL_PARTITION_LITTLEFS2)
		return littlefs_part_write(in_partition, off_set, in_buf,
					   in_buf_len);

	nandx_mutex_lock(&nandx_lock);
	if ((*off_set + in_buf_len) >
		hal_partitions[in_partition].partition_length) {
		pr_warn("%s, operation exceed partition: %d,"
			"partition_len: 0x%x, off: %d, len: %d\n",
			__func__, in_partition,
			hal_partitions[in_partition].partition_length,
			*off_set, in_buf_len);
		ret = -EIO;
		goto out;
	}

	offset = *off_set + hal_partitions[in_partition].partition_start_addr;
	ret = mtk_nand_rw_operation(in_buf, offset, in_buf_len, &op_len, false);
	*off_set += op_len;

out:
	nandx_mutex_unlock(&nandx_lock);
	return ret;
}

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success, otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition,
			   hal_logic_partition_t *partition)
{
	memcpy(partition, &hal_partitions[in_partition], sizeof(hal_logic_partition_t));

	return 0;
}

/**
 * Check offset in partition at partition id is a bad block or not for nand
 *
 * @param[in]  in_partition    The target flash logical partition which should
 *			       be check
 * @param[in]  off_set         Point to the address in partition
 *
 * @return  0 : Good block, 1: Bad block, EIO : If an error occurred with any
 *		step
 */
int hal_flash_isbad (hal_partition_t in_partition, uint32_t off_set)
{
	int ret;

	nandx_mutex_lock(&nandx_lock);
	if (off_set > hal_partitions[in_partition].partition_length) {
		pr_warn("%s, operation exceed partition: %d,"
			"partition_len: 0x%x, off: %d\n",
			__func__, in_partition,
			hal_partitions[in_partition].partition_length,
			off_set);
		ret = -EIO;
		goto out;
	}
	off_set += hal_partitions[in_partition].partition_start_addr;
	ret = bbt_is_bad(&nandxi, off_set);

out:
	nandx_mutex_unlock(&nandx_lock);
	return ret;
}

/**
 * Markbad at offset in partition at partition id for nand
 *
 * @param[in]  in_partition    The target flash logical partition which should
 *			       be check
 * @param[in]  off_set         Point to the address in partition
 *
 * @return  0 : On success, others : On fail
 */
int hal_flash_markbad(hal_partition_t in_partition, uint32_t off_set)
{
	int ret = 0;

	if (off_set > hal_partitions[in_partition].partition_length) {
		pr_warn("%s, operation exceed partition: %d,"
			"partition_len: 0x%x, off: %d\n",
			__func__, in_partition,
			hal_partitions[in_partition].partition_length,
			off_set);
		return -EIO;
	}
	off_set += hal_partitions[in_partition].partition_start_addr;

	if(bbt_is_bad(&nandxi, off_set)) {
		return 0;
	} else {
		/* Mark block bad in BBT */
		ret = bbt_mark_bad(&nandxi, off_set, BBT_BLOCK_FACTORY_BAD);
	}

	return ret;
}

/**
 * Get the page size for nand
 *
 * @return page size
 */
uint32_t hal_flash_get_pgsize()
{
	return nandxi.page_size;
}

/**
 * Get the block size for nand
 *
 * @return block size
 */
uint32_t hal_flash_get_blksize()
{
	return nandxi.block_size;
}

int nand_init()
{
	int ret = 0;

	ret = mtk_nand_init();
	if (ret) {
		pr_err("nand init error (%d)!\n", ret);
		return ret;
	}

	ret = littlefs_map_init();
	if (ret) {
		pr_err("littlefs map init error (%d)!\n", ret);
		return ret;
	}

	return ret;

}


