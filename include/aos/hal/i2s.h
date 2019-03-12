/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_I2S_H
#define HAL_I2S_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define HAL_WAIT_FOREVER 0xFFFFFFFFU

/*
 * I2S mode
 */
typedef enum {
    MODE_SLAVE_TX,
    MODE_SLAVE_RX,
    MODE_MASTER_TX,
    MODE_MASTER_RX
} hal_i2s_mode_t;

/*
 * I2S standard
 */
typedef enum {
    STANDARD_PHILIPS,
    STANDARD_MSB,
    STANDARD_LSB,
    STANDARD_PCM_SHORT,
    STANDARD_PCM_LONG
} hal_i2s_std_t;

/*
 * I2S data format
 */
typedef enum {
    DATAFORMAT_16B,
    DATAFORMAT_16B_EXTENDED,
    DATAFORMAT_24B,
    DATAFORMAT_32B
} hal_i2s_data_format_t;

/*
 * I2S configuration
 */
typedef struct {
    uint32_t              freq;         /* I2S communication frequency */
    hal_i2s_mode_t        mode;         /* I2S operating mode */
    hal_i2s_std_t         standard;     /* I2S communication standard */
    hal_i2s_data_format_t data_format;  /* I2S communication data format */
} i2s_config_t;

/*
 * I2S device description
 */
typedef struct {
    uint8_t       port;   /* i2s port */
    i2s_config_t  config; /* i2s config */
    void         *priv;   /* priv data */
} i2s_dev_t;

/**
 * Initialises a I2S dev
 *
 * @param[in]  i2s  the dev which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_init(i2s_dev_t *i2s);

/**
 * Transmit data on a I2S dev
 *
 * @param[in]  i2s      the I2S dev
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_send(i2s_dev_t *i2s, const void *data, uint32_t size, uint32_t timeout);

/**
 * Receive data on a I2S dev
 *
 * @param[in]   i2s      the I2S dev
 * @param[out]  data     pointer to the buffer which will store incoming data
 * @param[in]   size     number of bytes to receive
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_recv(i2s_dev_t *i2s, void *data, uint32_t size, uint32_t timeout);

/**
 * Pause a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_pause(i2s_dev_t *i2s);

/**
 * Resume a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_resume(i2s_dev_t *i2s);

/**
 * Stop a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_stop(i2s_dev_t *i2s);

/**
 * Deinitialises a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_finalize(i2s_dev_t *i2s);

#ifdef __cplusplus
}
#endif

#endif /* HAL_I2S_H */

