/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_NOR_H
#define HAL_NOR_H

typedef struct {
    uint32_t block_size;        /* NOR memory block size number of bytes */
    uint32_t chip_size;         /* NOR memory chip size measured in number of blocks */
} nor_config_t;

typedef struct {
    uint32_t         base_addr;
    nor_config_t     config;
    void            *priv;  
} nor_dev_t;

/**
 * Initialises a nor flash interface
 *
 * @param[in]  nor  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_init(nor_dev_t *nor);

/**
 * Deinitialises a nor flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_finalize(nor_dev_t *nor);

/**
 * Read data from NOR memory
 *
 * @param[in]   nor         the interface which should be initialised
 * @param[out]  data        pointer to the buffer which will store incoming data
 * @param[in]   addr        nor memory address
 * @param[in]   len         the number of bytes to read 
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len);


/**
 * Write data to NOR memory
 *
 * @param[in]   nor         the interface which should be initialised
 * @param[in]   data        pointer to source buffer to write
 * @param[in]   addr        nor memory address
 * @param[in]   len         the number of bytes to write
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len);
 
/*
 * Erase the blocks of the NOR memory
 *
 * @param[in]   nor         the interface which should be initialised
 * @param[in]   addr        nor memory address
 * @param[in]   block_count the number of block to erase
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count);

/*
 * Erase the entire NOR chip
 *
 * @param[in]   nor         the interface which should be initialised
 * @param[in]   addr        nor memory address
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr);

#endif
