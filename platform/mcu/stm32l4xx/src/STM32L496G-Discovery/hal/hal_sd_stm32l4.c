/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal_sd_stm32l4.h"

SD_HandleTypeDef sd_handle;
static int sd_initialize = 0;

void HAL_SD_MspInit(SD_HandleTypeDef * hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hsd->Instance == SDMMC1) {
      /* Peripheral clock enable */
      __HAL_RCC_SDMMC1_CLK_ENABLE();
    
      GPIO_InitStruct.Pin = GPIO_PIN_2;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
      GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_9 
                            |GPIO_PIN_8;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{

  if(hsd->Instance == SDMMC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_9 
                          |GPIO_PIN_8);
  }

}


int32_t hal_sd_init(sd_dev_t *sd)
{
    if (!sd_initialize) {
        sd_handle.Instance                  = SDMMC1;
        sd_handle.Init.ClockEdge            = SDMMC_CLOCK_EDGE_RISING;
        sd_handle.Init.ClockBypass          = SDMMC_CLOCK_BYPASS_DISABLE;
        sd_handle.Init.ClockPowerSave       = SDMMC_CLOCK_POWER_SAVE_DISABLE;
        sd_handle.Init.BusWide              = SDMMC_BUS_WIDE_1B;
        sd_handle.Init.HardwareFlowControl  = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
        sd_handle.Init.ClockDiv             = SDMMC_TRANSFER_CLK_DIV;

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
        return HAL_OK;
    } else {
        return -1;
    }
}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{
    if (HAL_SD_WriteBlocks(sd->priv, data, blk_addr, blks, timeout) == HAL_OK) {
        return HAL_OK;
    } else {
        return -1;
    }
        
}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    if (HAL_SD_Erase(sd->priv, blk_start_addr, blk_end_addr) == HAL_OK) {
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

