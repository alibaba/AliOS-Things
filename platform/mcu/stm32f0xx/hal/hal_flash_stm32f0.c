/**
  ******************************************************************************
  * @file    hal_flash_stm32f0.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    05/01/2019
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  ******************************************************************************
  */

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#ifdef HAL_FLASH_MODULE_ENABLED

#include <string.h>
#include <k_api.h>
#include "aos/hal/flash.h"
#include "stm32f0xx_hal_flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];

/**
  * @brief  Gets the page of a given address
  * @param  address: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t address)
{
  int32_t page = -1;

  if (address < FLASH_BANK1_END)
  {
    /* Bank 1 */
    page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  return page;
}

/**
  * @brief  Erase FLASH memory page(s) at address.
  * @note   The range to erase shall not cross the bank boundary.
  * @param  In: address     Start address to erase from.
  * @param  In: len_bytes   Length to be erased.
  * @retval  0:  Success.
            -1:  Failure.
  */
int FLASH_unlock_erase(uint32_t address, uint32_t len_bytes)
{
    int rc = -1;
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef EraseInit;
    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = address;
    EraseInit.NbPages = GetPage(address + len_bytes - 1) - GetPage(address) + 1;
	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInit, &PageError) == HAL_OK) {
		rc = 0;
	} else {
		printf("Error %lu erasing at 0x%08lx\n", PageError, address);
	}
    return rc;
}

/**
  * @brief  Write to FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_write_at(uint32_t address, uint32_t *pData, uint32_t len_bytes)
{
    int i;
    int ret = -1;
    uint32_t TypeProgram = 0;
    uint32_t step = 0;

    /*Without external Vpp it should NOT be 8 bytes parallelism*/
    if (len_bytes % 4 == 0)
    {
        TypeProgram = FLASH_TYPEPROGRAM_WORD;
        step = 4;
    }
    else if (len_bytes % 2 == 0)
    {
        TypeProgram = FLASH_TYPEPROGRAM_HALFWORD;
        step = 2;
    }
    else
    {
        TypeProgram = FLASH_TYPEPROGRAM_DOUBLEWORD;
        step = 8;
    }

    for (i = 0; i < len_bytes; i += step)
    {
        __disable_irq();
        ret = HAL_FLASH_Program(TypeProgram,
                              address + i,
                              *(pData + (i / step)));
        __enable_irq();
        if (ret != HAL_OK)
        {
            break;
        }
    }

     /* Memory check */
    if (memcmp((uint8_t *)address, (uint8_t *)pData, len_bytes) != 0)
    {
        ret = -1;
    }
    else
    {
        ret = 0;
    }

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
	/*Flash reading can be in byte*/
	memcpy((uint8_t *)pData, (uint8_t *)address, len_bytes);
    ret = 0;
    return ret;
}

int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *) data;

    uint64_t *page_cache = NULL;
    page_cache = (uint64_t *)krhino_mm_alloc(FLASH_PAGE_SIZE);
    if (page_cache == NULL) {
        return -1;
    }
    memset(page_cache, 0, FLASH_PAGE_SIZE);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

    do {
        uint32_t fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        int fl_offset = dst_addr - fl_addr;
        int len = MIN(FLASH_PAGE_SIZE - fl_offset, size);

        /* Load from the flash into the cache */
        memcpy(page_cache, (void *) fl_addr, FLASH_PAGE_SIZE);
        /* Update the cache from the source */
        memcpy((uint8_t *)page_cache + fl_offset, src_addr, len);
        /* Erase the page, and write the cache */
        ret = FLASH_unlock_erase(fl_addr, FLASH_PAGE_SIZE);
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

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];

    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf , uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;

    real_pno = pno;
    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( real_pno, p_partition_info );
    start_addr = p_partition_info->partition_start_addr + *poff;
    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;
    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;

    real_pno = pno;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( real_pno, p_partition_info );

    if (poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length) {
        return -1;
    }
    start_addr = p_partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;

    real_pno = pno;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( real_pno, p_partition_info );
    if (size + off_set > p_partition_info->partition_length) {
        return -1;
    }

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);

    FLASH_unlock_erase(start_addr, size);
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
