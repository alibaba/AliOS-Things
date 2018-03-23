/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal.h"
#include "stm32l4xx_hal.h"
#include "hal_qspi_stm32l4.h"
#include "stm32l4xx_hal_qspi.h"

/* Init and deInit function for qspi1 */
static int32_t qspi1_init(qspi_dev_t *qspi);
static int32_t qspi1_DeInit(void);
static void qspi1_MspInit(void);
static void qspi1_DeMspInit(void);

/* function used to transform hal para to stm32l4 para */
static int32_t qspi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4);

/* handle for qspi */
QSPI_HandleTypeDef qspi1_handle;

int32_t hal_qspi_init(qspi_dev_t *qspi)
{
    int32_t ret = -1;

    if (qspi == NULL) {
        return -1;
    }

    /*init qspi handle*/
    memset(&qspi1_handle, 0, sizeof(qspi1_handle));

    switch (qspi->port) {
        case PORT_QSPI1:
            qspi->priv = &qspi1_handle;
            ret = qspi1_init(qspi);
            break;

    /* if other qspi exist add init code here */

        default:
            break;
    }

    return ret;
}

int32_t hal_qspi_send(qspi_dev_t *qspi, const uint8_t *data, uint32_t timeout)
{
    int32_t ret = -1;

    if((qspi != NULL) && (data != NULL)) {
        ret = HAL_QSPI_Transmit((QSPI_HandleTypeDef *)qspi->priv,
             (uint8_t *)data, timeout);
    }

    return ret;
}

int32_t hal_qspi_recv(qspi_dev_t *qspi, uint8_t *data, uint32_t timeout)
{
    int32_t ret = -1;

    if((qspi != NULL) && (data != NULL)) {
          ret = HAL_QSPI_Receive((QSPI_HandleTypeDef *)qspi->priv,
               (uint8_t *)data, timeout);
    }

    return ret;
}

int32_t hal_qspi_command(qspi_dev_t *qspi, qspi_cmd_t *cmd, uint32_t timeout)
{
    int32_t ret = -1;
    QSPI_CommandTypeDef sCommand;

    if (qspi == NULL) {
        return -1;
    }

    switch (cmd->instruction) {
        case CMD_QUAD_PAGE_PROG:
/********************* configure parameter accoding to device connectted****************
            sCommand.Address = cmd->address;
            sCommand.NbData  = cmd->size;
            sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
            sCommand.Instruction       = CMD_QUAD_PAGE_PROG;
            sCommand.AddressMode       = QSPI_ADDRESS_4_LINES;
            sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
            sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
            sCommand.DataMode          = QSPI_DATA_4_LINES;
            sCommand.DummyCycles       = 0;
            sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
            sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
            sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
****************************************************************************************/
            ret = HAL_QSPI_Command((QSPI_HandleTypeDef *)qspi->priv,
                  &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

            break;

    /* if other cmd exist add init code here */

        default:
            break;
    }

    return ret;
}

int32_t hal_qspi_autoPolling(qspi_dev_t *qspi, uint8_t cmd, uint32_t timeout)
{
    int32_t ret = -1;
    QSPI_CommandTypeDef     sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    if (qspi == NULL) {
        return -1;
    }

    switch (cmd) {
        case AP_GET_MEM_STATUS:
/********************* configure parameter accoding to device connectted****************
            sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
            sCommand.Instruction       = AP_GET_MEM_STATUS;
            sCommand.AddressMode       = QSPI_ADDRESS_NONE;
            sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
            sCommand.DataMode          = QSPI_DATA_1_LINE;
            sCommand.DummyCycles       = 0;
            sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
            sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
            sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

            sConfig.Match           = 0;
            sConfig.Mask            = MX25R6435F_SR_WIP;
            sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
            sConfig.StatusBytesSize = 1;
            sConfig.Interval        = 0x10;
            sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
****************************************************************************************/
            ret = HAL_QSPI_AutoPolling(&qspi1_handle, &sCommand, &sConfig, timeout);

            break;

        case AP_GET_WRITE_ENABLE_STATUS:
/********************* configure parameter accoding to device connectted****************
            sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
            sCommand.Instruction       = AP_GET_WRITE_ENABLE_STATUS;
            sCommand.AddressMode       = QSPI_ADDRESS_NONE;
            sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
            sCommand.DataMode          = QSPI_DATA_1_LINE;
            sCommand.DummyCycles       = 0;
            sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
            sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
            sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

            sConfig.Match           = MX25R6435F_SR_WEL;
            sConfig.Mask            = MX25R6435F_SR_WEL;
            sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
            sConfig.StatusBytesSize = 1;
            sConfig.Interval        = 0x10;
            sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
****************************************************************************************/
            ret = HAL_QSPI_AutoPolling(&qspi1_handle, &sCommand, &sConfig, timeout);

            break;

    /* if other cmd exist add init code here */

        default:
            break;
    }

    return ret;
}

int32_t hal_qspi_finalize(qspi_dev_t *qspi)
{
    int32_t ret = -1;

    if (qspi == NULL) {
        return -1;
    }

    switch (qspi->port) {
        case PORT_QSPI1:
            qspi->priv = &qspi1_handle;
            ret = qspi1_DeInit();
            break;

        default:
            break;
    }

    return ret;
}

int32_t qspi1_init(qspi_dev_t *qspi)
{
    int32_t ret = 0;

    ret = qspi_freq_transform(qspi->config.freq, &qspi1_handle.Init.ClockPrescaler);
    if (ret != 0) {
        return -1;
    }

    /* Initialize other parameters in struction SPI_InitTypeDef */

    /* init qspi */
    qspi1_MspInit();
    ret = HAL_QSPI_Init(&qspi1_handle);

    return ret;
}

int32_t qspi1_DeInit(void)
{
    int32_t ret = -1;

    /* spi1 deinitialization */
    ret = HAL_QSPI_DeInit(&qspi1_handle);
    qspi1_DeMspInit();

    return ret;
}

void qspi1_MspInit(void)
{
    /* Initialize qspi-related pins */

    /* Initialize interrupts if necessary */	
}

void qspi1_DeMspInit(void)
{
    /* Disable qspi-related pins */

    /* Disable interrupts if necessary */	
}

static int32_t qspi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4)
{
    int32_t	ret = 0;

    /* calc BaudRatePrescaler according to freq_hal */

    return ret;
}
