/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/qspi.h"
#include "board.h"

/**
 * Initialises the QSPI interface for a given QSPI device
 *
 * @param[in]  qspi  the spi device
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_init(qspi_dev_t *qspi)
{
	return HAL_OK;
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
	return HAL_OK;
}

/**
 * Qspi recv
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  data     qspi recv data
 * @param[in]   size     qspi recv data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_recv(qspi_dev_t *qspi, uint8_t *data, uint32_t timeout)
{
	return HAL_OK;
}

/**
 * Set qspi command
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  cmd      qspi cmd
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_command(qspi_dev_t *qspi, qspi_cmd_t *cmd, uint32_t timeout)
{
	return HAL_OK;
}

/**
 * Configure automatic polling mode to wait for processing
 *
 * @param[in]   qspi     the qspi device
 * @param[out]  cmd      qspi cmd
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_auto_polling(qspi_dev_t *qspi, uint32_t cmd, uint32_t timeout)
{
	return HAL_OK;
}

/**
 * De-initialises a QSPI interface
 *
 * @param[in]  qspi the QSPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_qspi_finalize(qspi_dev_t *qspi)
{
	return HAL_OK;
}
