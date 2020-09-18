/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/nor.h"


/**
 * Initialises a nor flash interface
 *
 * @param[in]  nor  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_init(nor_dev_t *nor)
{
	return 0;
}

/**
 * Deinitialises a nor flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_finalize(nor_dev_t *nor)
{
	return 0;
}

/**
 * Read data from NOR memory
 *
 * @param[in]   nor   the interface which should be initialised
 * @param[out]  data  pointer to the buffer which will store incoming data
 * @param[in]   addr  nor memory address
 * @param[in]   len   the number of bytes to read
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
	return 0;
}


/**
 * Write data to NOR memory
 *
 * @param[in]  nor   the interface which should be initialised
 * @param[in]  data  pointer to source buffer to write
 * @param[in]  addr  nor memory address
 * @param[in]  len   the number of bytes to write
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
	return 0;
}

/*
 * Erase the blocks of the NOR memory
 *
 * @param[in]  nor          the interface which should be initialised
 * @param[in]  addr         nor memory address
 * @param[in]  block_count  the number of block to erase
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count)
{
	return 0;
}

/*
 * Erase the entire NOR chip
 *
 * @param[in]  nor   the interface which should be initialised
 * @param[in]  addr  nor memory address
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr)
{
	return 0;
}

