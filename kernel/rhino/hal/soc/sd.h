/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_SD_H
#define HAL_SD_H

typedef enum {
    SD_STAT_RESET,
    SD_STAT_READY,
    SD_STAT_TIMEOUT,
    SD_STAT_BUSY,
    SD_STAT_PROGRAMMING,
    SD_STAT_RECEIVING,
    SD_STAT_TRANSFER,
    SD_STAT_ERR
} hal_sd_stat;

typedef struct {
    uint32_t blk_nums; /* sd total block nums */
    uint32_t blk_size; /* sd block size */
} hal_sd_info_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t bus_wide;  /* sd bus wide */
    uint32_t freq;      /* sd freq */
} sd_config_t;

typedef struct {
    uint8_t       port;    /* sd port */
    sd_config_t   config;  /* sd config */
    void         *priv;    /* priv data */
} sd_dev_t;

/**
 * Initialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_init(sd_dev_t *sd);

/**
 * Read sd blocks
 *
 * @param[in]   sd        the interface which should be initialised
 * @param[out]  data      pointer to the buffer which will store incoming data
 * @param[in]   blk_addr  sd blk addr
 * @param[in]   blks      sd blks
 * @param[in]   timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout);

/**
 * Write sd blocks
 *
 * @param[in]  sd        the interface which should be initialised
 * @param[in]  data      pointer to the buffer which will store incoming data
 * @param[in]  blk_addr  sd blk addr
 * @param[in]  blks      sd blks
 * @param[in]  timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout);

/**
 * Erase sd blocks
 *
 * @param[in]  sd              the interface which should be initialised
 * @param[in]  blk_start_addr  sd blocks start addr
 * @param[in]  blk_end_addr    sd blocks end addr
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr);

/**
 * Get sd state
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat);

/**
 * Get sd info
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info);

/**
 * Deinitialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_finalize(sd_dev_t *sd);

#endif /* HAL_SD_H */

