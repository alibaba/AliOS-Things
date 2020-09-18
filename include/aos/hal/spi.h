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

#define DEFAULT_SPI_SERAIL_LEN 280

typedef enum
{
    SPI_ROLE_SLAVE,
    SPI_ROLE_MASTER,
} spi_role_e;

typedef enum
{
    SPI_FIRSTBIT_MSB,
    SPI_FIRSTBIT_LSB,
} spi_firstbit_e;

typedef enum
{
    SPI_WORK_MODE_0,                  // CPOL = 0; CPHA = 0
    SPI_WORK_MODE_2,                  // CPOL = 1; CPHA = 0
    SPI_WORK_MODE_1,                  // CPOL = 0; CPHA = 1
    SPI_WORK_MODE_3,                  // CPOL = 1; CPHA = 1
} spi_work_mode_e;

typedef enum
{
    SPI_TRANSFER_DMA,
    SPI_TRANSFER_NORMAL,
} spi_transfer_mode_e;

/* size of single spi frame data */
typedef enum
{
    SPI_DATA_SIZE_4BIT = 4,
    SPI_DATA_SIZE_5BIT,
    SPI_DATA_SIZE_6BIT,
    SPI_DATA_SIZE_7BIT,
    SPI_DATA_SIZE_8BIT,
    SPI_DATA_SIZE_9BIT,
    SPI_DATA_SIZE_10BIT,
    SPI_DATA_SIZE_11BIT,
    SPI_DATA_SIZE_12BIT,
    SPI_DATA_SIZE_13BIT,
    SPI_DATA_SIZE_14BIT,
    SPI_DATA_SIZE_15BIT,
    SPI_DATA_SIZE_16BIT,
} spi_data_size_e;

/* cs signal to active for transfer */
typedef enum
{
    SPI_CS_DIS,
    SPI_CS_EN,
} spi_cs_e;

/* Define spi config args */
typedef struct
{
    spi_role_e      role; /* spi communication mode */
    spi_firstbit_e  firstbit;
    spi_work_mode_e mode;
    spi_transfer_mode_e t_mode;
    uint32_t        freq; /* communication frequency Hz */
    uint16_t        serial_len; /* serial frame length, necessary for SPI running as Slave */
	spi_data_size_e data_size;
	spi_cs_e        cs;
} spi_config_t;

/* Define spi dev handle */
typedef struct {
    uint8_t       port;   /**< spi port */
    spi_config_t  config; /**< spi config */
    void         *priv;   /**< priv data */
} spi_dev_t;

typedef struct{
    spi_work_mode_e work_mode;
}spi_attribute_t;

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
 * spi send data and then recv data
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  the data to be sent
 * @param[in]  tx_size  data size to be sent
 * @param[out] rx_data  spi recv data
 * @param[in]  rx_size  data size to be recived
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0, on success,  otherwise is error
*/
int32_t hal_spi_send_and_recv(spi_dev_t *spi, uint8_t *tx_data, uint16_t tx_size, uint8_t *rx_data,
                              uint16_t rx_size, uint32_t timeout);

/**
 * spi send data and then send data
 * @param[in]  spi       the spi device
 * @param[in]  tx1_data  the first data to be sent
 * @param[in]  tx1_size  the first data size to be sent
 * @param[out] tx2_data  the second data to be sent
 * @param[in]  tx2_size  the second data size to be sent
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0, on success,  otherwise is error
 */
 int32_t hal_spi_send_and_send(spi_dev_t *spi, uint8_t *tx1_data, uint16_t tx1_size, uint8_t *tx2_data,
                               uint16_t tx2_size, uint32_t timeout);

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
void _hal_spi_test(void);
#ifdef __cplusplus
}
#endif

#endif /* HAL_SPI_H */