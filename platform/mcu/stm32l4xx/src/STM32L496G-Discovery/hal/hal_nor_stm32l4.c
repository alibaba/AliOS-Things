/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
  * Due to FMC is used for external flash, So it's just a sample!!!
  */

#include "hal_nor_stm32l4.h"

static NOR_HandleTypeDef nor_handle;
static int nor_initialize = 0;

int32_t hal_nor_init(nor_dev_t *nor)
{
    FMC_NORSRAM_TimingTypeDef Timing;

    if (nor_initialize)
        return HAL_OK;

    nor_handle.Instance = NOR_HANDLE_INSTANCE;
    nor_handle.Extended = NOR_HANDLE_EXTENDED;

    Timing.AddressSetupTime         = NOR_ADDR_SETUP_TIME;
    Timing.AddressHoldTime          = NOR_ADDR_HOLD_TIME;
    Timing.DataSetupTime            = NOR_DATA_SETUP_TIME;
    Timing.BusTurnAroundDuration    = NOR_BUS_TURN_DURATION;
    Timing.CLKDivision              = NOR_CLK_DIVISION;
    Timing.DataLatency              = NOR_DATA_LATENCY;
    Timing.AccessMode               = NOR_ACCESS_MODE;

    nor_handle.Init.NSBank               = NOR_INIT_NSBANK;
    nor_handle.Init.DataAddressMux       = NOR_INIT_DATA_ADDR_MUX;
    nor_handle.Init.MemoryType           = NOR_INIT_MEMORY_TYPE;
    nor_handle.Init.MemoryDataWidth      = NOR_INIT_MEM_DATA_WIDTH;
    nor_handle.Init.BurstAccessMode      = NOR_INIT_BURST_ACCESS_MODE;
    nor_handle.Init.WaitSignalPolarity   = NOR_INIT_WAIT_SIGNAL_PRI;
    nor_handle.Init.WaitSignalActive     = NOR_INIT_WAIT_SIGNAL_ACT;
    nor_handle.Init.WriteOperation       = NOR_INIT_WRITE_OPERATION;
    nor_handle.Init.WaitSignal           = NOR_INIT_WAIT_SIGNAL;
    nor_handle.Init.ExtendedMode         = NOR_INIT_EXTENDED_MODE;
    nor_handle.Init.AsynchronousWait     = NOR_INIT_ASYNC_WAIT;
    nor_handle.Init.WriteBurst           = NOR_INIT_WRITE_BURST;
    nor_handle.Init.ContinuousClock      = NOR_INIT_CONTINUOUS_CLK;
    nor_handle.Init.PageSize             = NOR_INIT_PAGE_SIZE;

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
        return -1;
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


