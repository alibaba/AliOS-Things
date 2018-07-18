/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"

#ifdef HAL_FLASH_MODULE_ENABLED

#include <k_api.h>
#include "hal/soc/soc.h"
#include "stm32l4xx_hal_flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];

/**
  * @brief  Get the bank of a given address.
  * @param  In: addr      Address in the FLASH Memory.
  * @retval Bank identifier.
  *           FLASH_BANK_1
  *           FLASH_BANK_2
  */
uint32_t FLASH_get_bank(uint32_t addr)
{
    uint32_t bank = 0;

#if defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || \
    defined (STM32L485xx) || defined (STM32L486xx) || defined (STM32L496xx) || \
    defined (STM32L4A6xx) || defined (STM32L4R5xx) || defined (STM32L4R7xx) || \
    defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
        /* No Bank swap */
        bank = (addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_1 : FLASH_BANK_2;
    } else {
        /* Bank swap */
        bank = (addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_2 : FLASH_BANK_1;
    }

#else
    bank = FLASH_BANK_1;
#endif

    return bank;
}

/**
  * @brief  Get the page of a given address within its FLASH bank.
  * @param  In: addr    Address in the FLASH memory.
  * @retval >=0 Success: Page number.
  *          <0 Failure: The address in not mapped in the internal FLASH memory.
  */
uint32_t FLASH_get_pageInBank(uint32_t addr)
{
    uint32_t page = 0xffffffff;

    if ( ((FLASH_BASE + FLASH_SIZE) > addr) && (addr >= FLASH_BASE) ) {
        /* The address is in internal FLASH range. */
        if ( addr < (FLASH_BASE + FLASH_BANK_SIZE) ) {
            /* Addr in the first bank */
            page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
        } else {
            /* Addr in the second bank */
            page = (addr - FLASH_BASE - FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
        }
    }

    return page;
}

#ifdef USING_FLAT_FLASH
uint32_t FLASH_flat_addr(uint32_t addr)
{
    uint32_t flat_addr = 0;

    if (addr < FLASH_BASE || addr >= FLASH_BASE + FLASH_SIZE) {
        printf("Error: Address is invalid.\n");
        return addr;
    }

#if defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || \
    defined (STM32L485xx) || defined (STM32L486xx) || defined (STM32L496xx) || \
    defined (STM32L4A6xx) || defined (STM32L4R5xx) || defined (STM32L4R7xx) || \
    defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
        /* Boot in bank1 */
        flat_addr = addr;
    } else {
        /* Boot in bank2 */
        flat_addr = addr < (FLASH_BASE + FLASH_BANK_SIZE) ? addr + FLASH_BANK_SIZE : addr - FLASH_BANK_SIZE;
    }

#else
    flat_addr = addr;
#endif

    return flat_addr;
}
#endif

/**
  * @brief  Erase FLASH memory page(s) at address.
  * @note   The range to erase shall not cross the bank boundary.
  * @param  In: address     Start address to erase from.
  * @param  In: len_bytes   Length to be erased.
  * @retval  0:  Success.
            -1:  Failure.
  */
int FLASH_erase_at(uint32_t address, uint32_t len_bytes)
{
    int ret = -1;
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef EraseInit;

    /* L4 ROM memory map, with 2 banks split into 2kBytes pages.
     * WARN: ABW. If the passed address and size are not page-aligned,
     * the start of the first page and the end of the last page are erased anyway.
     * After erase, the flash is left in unlocked state.
     */
    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;

    EraseInit.Banks = FLASH_get_bank(address);
    if (EraseInit.Banks != FLASH_get_bank(address + len_bytes - 1)) {
        printf("Error: Cannot erase across FLASH banks.\n");
    } else {
        EraseInit.Page = FLASH_get_pageInBank(address);
        EraseInit.NbPages = FLASH_get_pageInBank(address + len_bytes - 1) - EraseInit.Page + 1;

        ret = HAL_FLASHEx_Erase(&EraseInit, &PageError);
        if (ret == HAL_OK) {
            ret = 0;
        } else {
            printf("Error erasing at 0x%08lx\n", address);
        }
    }

    return ret;
}

/**
  * @brief  Write to FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed: Must be 8 byte aligned.
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_write_at(uint32_t address, uint64_t *pData, uint32_t len_bytes)
{
    int i;
    int ret = -1;

#ifndef CODE_UNDER_FIREWALL
    /* irq already mask under firewall */
    __disable_irq();
#endif

    for (i = 0; i < len_bytes; i += 8) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                              address + i,
                              *(pData + (i / 8) )) != HAL_OK) {
            break;
        }
    }
    /* Memory check */
    for (i = 0; i < len_bytes; i += 4) {
        uint32_t *dst = (uint32_t *)(address + i);
        uint32_t *src = ((uint32_t *) pData) + (i / 4);

        if ( *dst != *src ) {
#ifndef CODE_UNDER_FIREWALL
            printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) dst, *dst, *src);
#endif
            break;
        }
        ret = 0;
    }
#ifndef CODE_UNDER_FIREWALL
    /* irq should never be enable under firewall */
    __enable_irq();
#endif
    return ret;
}

/*****************************************************************/
/**
  * @brief  Read from FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed: Must be 8 byte aligned.
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes)
{
    int i;
    int ret = -1;
    uint32_t *src = (uint32_t *)(address);
    uint32_t *dst = ((uint32_t *) pData);


    for (i = 0; i < len_bytes; i += 4) {
        *(dst + i / 4) = *(src++);
    }

    ret = 0;
    return ret;
}

int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *) data;
    uint32_t fl_addr = 0;
    int fl_offset = 0;
    int len = 0;
    uint64_t *page_cache = NULL;

    page_cache = (uint64_t *)krhino_mm_alloc(FLASH_PAGE_SIZE);
    if (page_cache == NULL) {
        return -1;
    }
    memset(page_cache, 0, FLASH_PAGE_SIZE);

    do {
        fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        fl_offset = dst_addr - fl_addr;
        len = MIN(FLASH_PAGE_SIZE - fl_offset, remaining);

        /* Load from the flash into the cache */
        memcpy(page_cache, (void *) fl_addr, FLASH_PAGE_SIZE);
        /* Update the cache from the source */
        memcpy((uint8_t *)page_cache + fl_offset, src_addr, len);
        /* Erase the page, and write the cache */
        ret = FLASH_erase_at(fl_addr, FLASH_PAGE_SIZE);
        if (ret != 0) {
            printf("Error erasing at 0x%08lx\n", fl_addr);
        } else {
            ret = FLASH_write_at(fl_addr, page_cache, FLASH_PAGE_SIZE);
            if (ret != 0) {
                printf("Error writing %lu bytes at 0x%08lx\n", FLASH_PAGE_SIZE, fl_addr);
            } else {
                dst_addr += len;
                src_addr += len;
                remaining -= len;
            }
        }
    } while ((ret == 0) && (remaining > 0));

    krhino_mm_free(page_cache);
    return ret;
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf , uint32_t buf_size)
{
    int ret = 0;
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

    ret = HAL_FLASH_Unlock();
    if (ret != 0) {
        printf("HAL_FLASH_Unlock return %d\n", ret);
        return -1;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    real_pno = pno;
    partition_info = hal_flash_get_info( real_pno );
    start_addr = partition_info->partition_start_addr + *poff;

#ifdef USING_FLAT_FLASH
    start_addr = FLASH_flat_addr(start_addr);
#endif

    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;

    ret = HAL_FLASH_Lock();
    if (ret != 0) {
        printf("HAL_FLASH_Lock return %d\n", ret);
        return -1;
    }

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );

    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length) {
        return -1;
    }
    start_addr = partition_info->partition_start_addr + *poff;

#ifdef USING_FLAT_FLASH
    start_addr = FLASH_flat_addr(start_addr);
#endif

    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    int ret = 0;
    uint32_t start_addr;
    uint32_t erase_size;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

    ret = HAL_FLASH_Unlock();
    if (ret != 0) {
        printf("HAL_FLASH_Unlock return %d\n", ret);
        return -1;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    real_pno = pno;
    partition_info = hal_flash_get_info( real_pno );
    if (size + off_set > partition_info->partition_length) {
        return -1;
    }

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);
    erase_size = partition_info->partition_start_addr + off_set - start_addr + size;

#ifdef USING_FLAT_FLASH
    start_addr = FLASH_flat_addr(start_addr);
#endif

    ret = FLASH_erase_at(start_addr, erase_size);
    if (ret != 0) {
        printf("FLASH_erase_at return failed\n");
    }

    ret = HAL_FLASH_Lock();
    if (ret != 0) {
        printf("HAL_FLASH_Lock return %d\n", ret);
        return -1;
    }

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

#endif
