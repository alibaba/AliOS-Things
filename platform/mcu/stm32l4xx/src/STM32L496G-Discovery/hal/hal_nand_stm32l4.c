/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
  * Due to FMC is used for external flash, So it's just a sample!!!
  */

#include "hal_nand_stm32l4.h"

NAND_HandleTypeDef nand_handle;
static uint32_t nand_initialize = 0;
static uint32_t FMC_Initialized = 0;

void HAL_NAND_MspInit(NAND_HandleTypeDef* hnand)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    if (FMC_Initialized) {
      return;
    }
    FMC_Initialized = 1;
    /* Peripheral clock enable */
    __HAL_RCC_FMC_CLK_ENABLE();
    
    /** FMC GPIO Configuration  
    PG9   ------> FMC_NCE
    PD0   ------> FMC_D2
    PD4   ------> FMC_NOE
    PD1   ------> FMC_D3
    PD5   ------> FMC_NWE
    PD6   ------> FMC_NWAIT
    PE10   ------> FMC_D7
    PD15   ------> FMC_D1
    PE9   ------> FMC_D6
    PE8   ------> FMC_D5
    PD14   ------> FMC_D0
    PD12   ------> FMC_ALE
    PD11   ------> FMC_CLE
    PE7   ------> FMC_D4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_1|GPIO_PIN_5 
                            |GPIO_PIN_6|GPIO_PIN_15|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


int32_t hal_nand_init(nand_dev_t *nand)
{
    FMC_NAND_PCC_TimingTypeDef ComSpaceTiming;
    FMC_NAND_PCC_TimingTypeDef AttSpaceTiming;

    if (nand_initialize)
        return HAL_OK;

    /** Perform the NAND1 memory initialization sequence
        */
    nand_handle.Instance = FMC_NAND_DEVICE;
    /* hnand1.Init */
    nand_handle.Init.NandBank = FMC_NAND_BANK3;
    nand_handle.Init.Waitfeature = FMC_NAND_WAIT_FEATURE_ENABLE;
    nand_handle.Init.MemoryDataWidth = FMC_NAND_MEM_BUS_WIDTH_8;
    nand_handle.Init.EccComputation = FMC_NAND_ECC_ENABLE;
    nand_handle.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_512BYTE;
    nand_handle.Init.TCLRSetupTime = 0;
    nand_handle.Init.TARSetupTime = 0;
    /* hnand1.Info */
    nand_handle.Info.PageSize = 0x800;
    nand_handle.Info.SpareAreaSize = 0x40;
    nand_handle.Info.BlockSize = 0x40;
    nand_handle.Info.BlockNbr = 0x400;
    nand_handle.Info.ZoneSize = 0x400;
    /* ComSpaceTiming */
    ComSpaceTiming.SetupTime = 0x5;
    ComSpaceTiming.WaitSetupTime = 0x4;
    ComSpaceTiming.HoldSetupTime = 0x2;
    ComSpaceTiming.HiZSetupTime = 0x5;
    /* AttSpaceTiming */
    AttSpaceTiming.SetupTime = 0x5;
    AttSpaceTiming.WaitSetupTime = 0x4;
    AttSpaceTiming.HoldSetupTime = 0x2;
    AttSpaceTiming.HiZSetupTime = 0x5;

    if (HAL_NAND_Init(&nand_handle, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
        return -1;

    nand_initialize = 1;
    nand->priv = (void *)&nand_handle;
    nand->config.page_size          = nand_handle.Info.PageSize;
    nand->config.spare_area_size    = nand_handle.Info.SpareAreaSize;
    nand->config.block_size         = nand_handle.Info.BlockSize;
    nand->config.zone_size          = nand_handle.Info.ZoneSize;
    nand->config.zone_number        = 1;

    return HAL_OK;
}

int32_t hal_nand_finalize(nand_dev_t *nand)
{
    int32_t ret = HAL_OK;

    nand_initialize = 0;    

    if (HAL_NAND_DeInit(&nand_handle) != HAL_OK) {
        ret = -1;
    }

    memset(nand, 0, sizeof(nand_dev_t));

    return -1;
}

int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
    NAND_AddressTypeDef nand_addr;
    nand_addr.Block = addr->block;
    nand_addr.Page = addr->page;
    nand_addr.Zone = addr->zone;
 
    if (HAL_NAND_Read_Page(nand->priv, &nand_addr, data, page_count) != HAL_OK)
        return -1;

    return HAL_OK;
}

int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{
    NAND_AddressTypeDef nand_addr;
    nand_addr.Block = addr->block;
    nand_addr.Page = addr->page;
    nand_addr.Zone = addr->zone;
 
    if (HAL_NAND_Write_Page(nand->priv, &nand_addr, data, page_count) != HAL_OK)
        return -1;

    return HAL_OK;
}

int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
    NAND_AddressTypeDef nand_addr;
    nand_addr.Block = addr->block;
    nand_addr.Page = addr->page;
    nand_addr.Zone = addr->zone;
 
    if (HAL_NAND_Read_SpareArea(nand->priv, &nand_addr, data, data_len) != HAL_OK)
        return -1;

    return HAL_OK;
}

int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{
    NAND_AddressTypeDef nand_addr;
    nand_addr.Block = addr->block;
    nand_addr.Page = addr->page;
    nand_addr.Zone = addr->zone;
 
    if (HAL_NAND_Write_SpareArea(nand->priv, &nand_addr, data, data_len) != HAL_OK)
        return -1;

    return HAL_OK;
}

int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr)
{
    NAND_AddressTypeDef nand_addr;
    nand_addr.Block = addr->block;
    nand_addr.Page = addr->page;
    nand_addr.Zone = addr->zone;
 
    if (HAL_NAND_Erase_Block(nand->priv, &nand_addr) != HAL_OK)
        return -1;

    return HAL_OK;
}

