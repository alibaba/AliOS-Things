/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_NAND_H
#define HAL_NAND_H

typedef struct {
    uint32_t page_size;         /* NAND memory page size w/o spare area */
    uint32_t spare_area_size;   /* NAND memory spare area size */
    uint32_t block_size;        /* NAND memory block size number of pages */
    uint32_t zone_size;         /* NAND memory zone size measured in number of blocks */
    uint32_t zone_number;       /* NAND memory number of zones */
} nand_config_t;

typedef struct {
    uint16_t page;      /* NAND memory Page address */
    uint16_t block;     /* NAND memory Block address */
    uint16_t zone;      /* NAND memory Zone address */
} nand_addr_t;

typedef struct {
    uint32_t         base_addr;
    nand_config_t    config;
    void            *priv;  
} nand_dev_t;

/**
 * Initialises a nand flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_init(nand_dev_t *nand);

/**
 * Deinitialises a nand flash interface
 *
 * @param[in]  nand  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_finalize(nand_dev_t *nand);

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
int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count);


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
int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count);

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
int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len);


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
int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len);

/**
 * Erase nand block
 *
 * @param[in]   nand      the interface which should be initialised
 * @param[in]   addr      nand address
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr);
#endif
