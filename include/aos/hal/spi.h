/**
 * @file spi.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_spi SPI
 *  spi hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#define HAL_SPI_MODE_MASTER 1 /**< spi communication is master mode */
#define HAL_SPI_MODE_SLAVE  2 /**< spi communication is slave mode */

/* Define spi config args */
typedef struct {
    uint32_t mode; /**< spi communication mode */
    uint32_t freq; /**< communication frequency Hz */
} spi_config_t;

/* Define spi dev handle */
typedef struct {
    uint8_t       port;   /**< spi port */
    spi_config_t  config; /**< spi config */
    void         *priv;   /**< priv data */
} spi_dev_t;

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_init(spi_dev_t *spi);

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[out] rx_data  spi recv data
 * @param[in]  size     spi data to be sent and recived
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0, on success,  otherwise is error
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout);

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi  the SPI device to be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_finalize(spi_dev_t *spi);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_SPI_H */

