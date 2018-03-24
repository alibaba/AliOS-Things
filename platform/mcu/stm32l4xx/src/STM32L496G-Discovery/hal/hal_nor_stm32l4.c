/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
  * Due to FMC is used for external flash, So it's just a sample!!!
  */

#include "hal_nor_stm32l4.h"

static NOR_HandleTypeDef nor_handle;
static int nor_initialize = 0;

void HAL_NOR_MspInit(NOR_HandleTypeDef *hnor)
{
  GPIO_InitTypeDef gpioinitstruct = {0};
  
  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();
  
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  
  /* Common GPIO configuration */
  gpioinitstruct.Mode      = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull      = GPIO_PULLUP;
  gpioinitstruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpioinitstruct.Alternate = GPIO_AF12_FMC;
  
  /*## Data Bus #######*/
  /* GPIOD configuration */
  gpioinitstruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                              GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
   
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* GPIOE configuration */  
  gpioinitstruct.Pin   = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                              GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpioinitstruct);
  
  /*## Address Bus #######*/
  /* GPIOF configuration */  
  gpioinitstruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |
                              GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpioinitstruct);
  
  /* GPIOG configuration */  
  gpioinitstruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOG, &gpioinitstruct);
  
  /* GPIOD configuration */
  gpioinitstruct.Pin   = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* GPIOE configuration */  
  gpioinitstruct.Pin   = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOE, &gpioinitstruct);

  /*## NOE and NWE configuration ######*/
  gpioinitstruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /*## NE2/NCE3 configuration ######*/
  gpioinitstruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOG, &gpioinitstruct);
  
  /*## Configure PD6 for NOR memory Ready/Busy signal ######*/
  gpioinitstruct.Pin       = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);
}


int32_t hal_nor_init(nor_dev_t *nor)
{
    FMC_NORSRAM_TimingTypeDef Timing;

    if (nor_initialize)
        return HAL_OK;

    nor_handle.Instance = FMC_NORSRAM_DEVICE;
    nor_handle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

    Timing.AddressSetupTime         = 1;
    Timing.AddressHoldTime          = 1;
    Timing.DataSetupTime            = 6;
    Timing.BusTurnAroundDuration    = 0;
    Timing.CLKDivision              = 2;
    Timing.DataLatency              = 2;
    Timing.AccessMode               = FMC_ACCESS_MODE_B;

    nor_handle.Init.NSBank               = FMC_NORSRAM_BANK2;
    nor_handle.Init.DataAddressMux       = FMC_DATA_ADDRESS_MUX_DISABLE;
    nor_handle.Init.MemoryType           = FMC_MEMORY_TYPE_NOR;
    nor_handle.Init.MemoryDataWidth      = FMC_NORSRAM_MEM_BUS_WIDTH_16;
    nor_handle.Init.BurstAccessMode      = FMC_BURST_ACCESS_MODE_DISABLE;
    nor_handle.Init.WaitSignalPolarity   = FMC_WAIT_SIGNAL_POLARITY_LOW;
    nor_handle.Init.WaitSignalActive     = FMC_WAIT_TIMING_BEFORE_WS;
    nor_handle.Init.WriteOperation       = FMC_WRITE_OPERATION_ENABLE;
    nor_handle.Init.WaitSignal           = FMC_WAIT_SIGNAL_DISABLE;
    nor_handle.Init.ExtendedMode         = FMC_EXTENDED_MODE_DISABLE;
    nor_handle.Init.AsynchronousWait     = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    nor_handle.Init.WriteBurst           = FMC_WRITE_BURST_DISABLE;
    nor_handle.Init.ContinuousClock      = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
    nor_handle.Init.PageSize             = FMC_PAGE_SIZE_NONE;

    if(HAL_NOR_Init(&nor_handle, &Timing, &Timing) != HAL_OK) {
        return -1;
    }

    nor_initialize = 1;
    
    nor->priv = (void *)&nor_handle;
    nor->config.block_size = NOR_BLOCK_SIZE;

    return HAL_OK;
}

int32_t hal_nor_finalize(nor_dev_t *nor)
{
    int32_t ret = HAL_OK;

    nor_initialize = 0;    

    if (HAL_NOR_DeInit(&nor_handle) != HAL_OK) {
        ret = -1;
    }

    memset(nor, 0, sizeof(nor_dev_t));

    return -1;

}

int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    if (HAL_NOR_ReadBuffer(nor->priv, NOR_DEVICE_ADDR + addr, data, len) != HAL_OK)
        return -1
    else
        return HAL_OK;
}

int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    uint32_t index = len;

    while(index > 0) {
        HAL_NOR_Program(nor->priv, (uint32_t *)(NOR_DEVICE_ADDR + addr), data);

        if (HAL_NOR_GetStatus(nor->priv, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != HAL_NOR_STATUS_SUCCESS) {
            return -1;
        }

        index--;
        addr += 2;
        data++;
    }

    return HAL_OK;
}

int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count)
{
    uint32_t index = block_count;

    while(index > 0) {
        HAL_NOR_Erase_Block(nor->priv, addr, NOR_DEVICE_ADDR);
  
        /* Return the NOR memory status */  
        if(HAL_NOR_GetStatus(nor->priv, NOR_DEVICE_ADDR, BLOCKERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS) {
            return -1;
        }
        index--;
        addr += nor->config.block_size;
    }

    return HAL_OK;
}

int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr)
{
    /* Send NOR Erase chip operation */
    HAL_NOR_Erase_Chip(nor->priv, NOR_DEVICE_ADDR);
  
    /* Return the NOR memory status */
    if(HAL_NOR_GetStatus(nor->priv, NOR_DEVICE_ADDR, CHIPERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS) {
        return -1;
    } else {
        return HAL_OK;
    } 

}


