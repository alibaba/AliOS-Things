/**
 * @file qspi.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_QSPI_H
#define HAL_QSPI_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_qspi QSPI
 *  qspi hal API.
 *
 *  @{
 */

#include <stdint.h>
#include <stdbool.h>

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#define DEFAULT_QSPI_FREQ 400000

/* cs signal to active for transfer */
typedef enum
{
    QSPI_CS_DIS,
    QSPI_CS_L,
    QSPI_CS_H,
} qspi_cs_e;

typedef enum
{
    QSPI_WORK_MODE_0,                  // CPOL = 0; CPHA = 0
    QSPI_WORK_MODE_1,                  // CPOL = 0; CPHA = 1
    QSPI_WORK_MODE_2,                  // CPOL = 1; CPHA = 0
    QSPI_WORK_MODE_3,                  // CPOL = 1; CPHA = 1
} qspi_work_mode_e;

typedef enum
{
    QSPI_TRANSFER_MODE_DMA,
    QSPI_TRANSFER_MODE_NORMAL,
} qspi_transfer_mode_e;

typedef struct {
    struct {
        uint32_t skip:1;              /**< Instruction phase skipped if disabled is set to true */
        uint32_t width:3;     /**< Bus width for the instruction */
        uint32_t value:8;             /**< Instruction value */
    } instruction;
    struct {
        uint32_t skip:1;              /**< Address phase skipped if disabled is set to true */
        uint32_t width:3;     /**< Bus width for the address */
        uint32_t size:3;   /**< Address size */
        uint32_t value;              /**< Address value */
    }  address;
    struct {
        uint32_t skip:1;              /**< Alternative phase skipped if disabled is set to true */
        uint32_t width:3;     /**< Bus width for alternative */
        uint32_t size:3;  /**< Alternative size */
        uint32_t value;               /**< Alternative value */
    } alt;
    uint8_t dummy_count;              /**< Dummy cycles count */
    struct {
        uint32_t width;       /**< Bus width for data */
    } data;
} qspi_cmd_t;

typedef struct {
    qspi_cs_e        cs;
    qspi_work_mode_e mode;
    qspi_transfer_mode_e t_mode;
    uint32_t freq;             /**< communication frequency Hz */
    uint32_t bus_width;
    bool ddr_enable;           /**< SDR mode or DDR mode */
} qspi_config_t;

typedef struct {
    uint8_t        port;   /**< qspi port */
    qspi_config_t  config; /**< qspi config */
    void          *priv;   /**< priv data */
} qspi_dev_t;

typedef struct  {
    const uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint16_t tx_size;
    uint16_t rx_size;
    uint32_t timeout;
} qspi_data_t;


typedef struct {
    qspi_cmd_t *cmd;
    qspi_data_t *data;
} qspi_transfer_t;


/**
 * Initialises the QSPI interface for a given QSPI device
 *
 * @param[in]  qspi  the spi device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_init (qspi_dev_t *qspi);

/**
 * Qspi send
 *
 * @param[in]  qspi     the qspi device
 * @param[in]  tx       tx command and data
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_send (qspi_dev_t *qspi, qspi_transfer_t *tx);

/**
 * Qspi recv
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  rx       rx command and data
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_recv (qspi_dev_t *qspi, qspi_transfer_t *rx);

/**
 * Qspi send and recv
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  trx      tx/rx command and data
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_send_recv (qspi_dev_t *qspi, qspi_transfer_t *trx);


/**
 * Set qspi command
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  cmd      qspi cmd
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_send_cmd (qspi_dev_t *qspi, qspi_cmd_t *cmd);

/**
 * De-initialises a QSPI interface
 *
 * @param[in]  qspi  the QSPI device to be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_qspi_finalize (qspi_dev_t *qspi);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_QSPI_H */

