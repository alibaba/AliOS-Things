/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/qspi.h"


/**
 * Initialises the QSPI interface for a given QSPI device
 *
 * @param[in]  qspi  the spi device
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_init(qspi_dev_t *qspi)
{
	return 0;
}

/**
 * Qspi send
 *
 * @param[in]  qspi     the qspi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_send(qspi_dev_t *qspi, const uint8_t *data, uint32_t timeout)
{
	return 0;
}

/**
 * Qspi recv
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  data     qspi recv data
 * @param[in]   size     qspi recv data size
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_recv(qspi_dev_t *qspi, uint8_t *data, uint32_t timeout)
{
	return 0;
}

/**
 * Set qspi command
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  cmd      qspi cmd
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_command(qspi_dev_t *qspi, qspi_cmd_t *cmd, uint32_t timeout)
{
	return 0;
}

/**
 * Configure automatic polling mode to wait for processing
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  cmd      qspi cmd
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_auto_polling(qspi_dev_t *qspi, uint32_t cmd, uint32_t timeout)
{
	return 0;
}

/**
 * De-initialises a QSPI interface
 *
 * @param[in]  qspi  the QSPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_finalize(qspi_dev_t *qspi)
{
	return 0;
}

