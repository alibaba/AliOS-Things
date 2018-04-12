/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if defined(HAL_SD_MODULE_ENABLED)
#if !defined(STM32L432xx)

#include "hal_sd_stm32l4.h"

static SD_HandleTypeDef sd_handle;
static int sd_initialize = 0;

int32_t hal_sd_init(sd_dev_t *sd)
{
    if (!sd_initialize) {
        sd_handle.Instance                  = SD_INSTANCE;
        sd_handle.Init.ClockEdge            = SD_INIT_CLK_EDGE;
        sd_handle.Init.ClockBypass          = SD_INIT_CLK_BYPASS;
        sd_handle.Init.ClockPowerSave       = SD_INIT_CLK_POWER_SAVE;
        sd_handle.Init.BusWide              = SD_INIT_BUS_WIDE;
        sd_handle.Init.HardwareFlowControl  = SD_INIT_HW_FLOW_CTRL;
        sd_handle.Init.ClockDiv             = SD_INIT_CLK_DIVISION;

        if (HAL_SD_Init(&sd_handle) != HAL_OK) {
            return -1;
        }

        if (HAL_SD_ConfigWideBusOperation(&sd_handle, SDMMC_BUS_WIDE_4B) != HAL_OK) {
            return -1;
        }

        sd_initialize = 1;
    }

    sd->priv = (void *)&sd_handle;

    return HAL_OK;
}

int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
{
    if (HAL_SD_ReadBlocks(sd->priv, data, blk_addr, blks, timeout) == HAL_OK) {
        while (HAL_SD_GetCardState(sd->priv) != HAL_SD_CARD_TRANSFER) {}
        return HAL_OK;
    } else {
        return -1;
    }
}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{
    if (HAL_SD_WriteBlocks(sd->priv, data, blk_addr, blks, timeout) == HAL_OK) {
        while (HAL_SD_GetCardState(sd->priv) != HAL_SD_CARD_TRANSFER) {}
        return HAL_OK;
    } else {
        return -1;
    }
        
}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    if (HAL_SD_Erase(sd->priv, blk_start_addr, blk_end_addr) == HAL_OK) {
        while (HAL_SD_GetCardState(sd->priv) != HAL_SD_CARD_TRANSFER) {}
        return HAL_OK;
    } else {
        return -1;
    }
}

int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
    HAL_SD_CardStateTypedef card_state;
    card_state = HAL_SD_GetCardState(sd->priv);

    switch (card_state) {
        case HAL_SD_CARD_TRANSFER:
            *stat = SD_STAT_TRANSFER;
            break;

        case HAL_SD_CARD_SENDING:
        case HAL_SD_CARD_RECEIVING:
        case HAL_SD_CARD_PROGRAMMING:
            *stat = SD_STAT_BUSY;
            break;

        default:
            *stat = SD_STAT_ERR;
            break;
    }
    return 0;
}


int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
    SD_HandleTypeDef *psd = (SD_HandleTypeDef *)(sd->priv);
    info->blk_nums = psd->SdCard.BlockNbr;
    info->blk_size = psd->SdCard.BlockSize;
    return HAL_OK;
}

int32_t hal_sd_finalize(sd_dev_t *sd)
{
    int32_t ret = HAL_OK;

    sd_initialize = 0;    
    sd_handle.Instance = SDMMC1;

    if (HAL_SD_DeInit(&sd_handle) != HAL_OK) {
        ret = -1;
    }

    memset(sd, 0, sizeof(sd_dev_t));

    return -1;
}

void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&sd_handle);
}

#endif
#endif
