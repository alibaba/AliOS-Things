/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
  * Due to FMC is used for external flash, So it's just a sample!!!
  */
#include "stm32f4xx_hal.h"
#include "hal_nand_stm32f4.h"
#include "aos/hal/nand.h"
#include "stm32f4xx_hal_nand.h"

#if defined(HAL_NAND_MODULE_ENABLED)
#if defined(STM32F429xx) || defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx) || \
    defined(STM32L496xx) || defined(STM32L4A6xx) || \
    defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)

#include "aos/hal/nand.h"

#include "hal_nand_stm32f4.h"

static NAND_HandleTypeDef nand_handle;
static uint32_t nand_initialize = 0;
static uint32_t FMC_Initialized = 0;

int32_t hal_nand_init(nand_dev_t *nand)
{
    FMC_NAND_PCC_TimingTypeDef ComSpaceTiming;
    FMC_NAND_PCC_TimingTypeDef AttSpaceTiming;

    if (nand_initialize)
        return HAL_OK;

    /** Perform the NAND1 memory initialization sequence
        */
    nand_handle.Instance = NAND_HANDLE_INSTANCE;
    /* hnand1.Init */
    nand_handle.Init.NandBank = NAND_INIT_NAND_BANK;
    nand_handle.Init.Waitfeature = NAND_INIT_WAIT_FEATURE;
    nand_handle.Init.MemoryDataWidth = NAND_INIT_MEMORY_DATA_WIDTH;
    nand_handle.Init.EccComputation = NAND_INIT_ECC_COMPUTATION;
    nand_handle.Init.ECCPageSize = NAND_INIT_ECC_PAGE_SIZE;
    nand_handle.Init.TCLRSetupTime = NAND_INIT_TCLR_SETUP_TIME;
    nand_handle.Init.TARSetupTime = NAND_INIT_TAR_SETUP_TIME;
    /* hnand1.Info */
    nand_handle.Info.PageSize = NAND_INFO_PAGE_SIZE;
    nand_handle.Info.SpareAreaSize = NAND_INFO_SPARE_AREA_SIZE;
    nand_handle.Info.BlockSize = NAND_INFO_BLOCK_SIZE;
    nand_handle.Info.BlockNbr = NAND_INFO_BLOCK_NBR;
    nand_handle.Info.ZoneSize = NAND_INFO_ZONE_SIZE;
    /* ComSpaceTiming */
    ComSpaceTiming.SetupTime = COM_SPACE_SETUP_TIME;
    ComSpaceTiming.WaitSetupTime = COM_SPACE_WAIT_TIME;
    ComSpaceTiming.HoldSetupTime = COM_SPACE_HOLD_TIME;
    ComSpaceTiming.HiZSetupTime = COM_SPACE_HIZ_TIME;
    /* AttSpaceTiming */
    AttSpaceTiming.SetupTime = ATT_SPACE_SETUP_TIME;
    AttSpaceTiming.WaitSetupTime = ATT_SPACE_WAIT_TIME;
    AttSpaceTiming.HoldSetupTime = ATT_SPACE_HOLD_TIME;
    AttSpaceTiming.HiZSetupTime = ATT_SPACE_HIZ_TIME;

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

#endif
#endif
