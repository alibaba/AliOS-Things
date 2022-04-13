/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/nand.h"

/**
 * Initialises a nand flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_init(nand_dev_t *nand)
{
	return 0;
}

/**
 * Deinitialises a nand flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_finalize(nand_dev_t *nand)
{
	return 0;
}

/**
 * Read nand page(s)
 *
 * @param[in]   nand        the interface which should be initialised
 * @param[out]  data        pointer to the buffer which will store incoming data
 * @param[in]   addr        nand address
 * @param[in]   page_count  the number of pages to read
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
	return 0;
}

/**
 * Write nand page(s)
 *
 * @param[in]   nand        the interface which should be initialised
 * @param[in]   data        pointer to source buffer to write
 * @param[in]   addr        nand address
 * @param[in]   page_count  the number of pages to write
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
	return 0;
}

/**
 * Read nand spare area
 *
 * @param[in]   nand      the interface which should be initialised
 * @param[out]  data      pointer to the buffer which will store incoming data
 * @param[in]   addr      nand address
 * @param[in]   data_len  the number of spares to read
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
	return 0;
}

/**
 * Write nand spare area
 *
 * @param[in]   nand      the interface which should be initialised
 * @param[in]   data      pointer to source buffer to write
 * @param[in]   addr      nand address
 * @param[in]   data_len  the number of spares to write
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
	return 0;
}

/**
 * Erase nand block
 *
 * @param[in]   nand      the interface which should be initialised
 * @param[in]   addr      nand address
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr)
{
	return 0;
}

