/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/qspi.h"

#ifdef HAL_QSPI_MODULE_ENABLED

int32_t hal_qspi_init(qspi_dev_t *qspi)
{

}

int32_t hal_qspi_send(qspi_dev_t *qspi, const uint8_t *data, uint32_t timeout)
{

}

int32_t hal_qspi_recv(qspi_dev_t *qspi, uint8_t *data, uint32_t timeout)
{

}

int32_t hal_qspi_command(qspi_dev_t *qspi, qspi_cmd_t *cmd, uint32_t timeout)
{

}

int32_t hal_qspi_auto_polling(qspi_dev_t *qspi, uint32_t cmd, uint32_t timeout)
{

}

int32_t hal_qspi_finalize(qspi_dev_t *qspi)
{

}

#endif /* HAL_QSPI_MODULE_ENABLED */

