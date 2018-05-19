/**
  ******************************************************************************
  * @file    Cloud/AWS/Src/flash.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   Management of the L4 internal flash memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include "stm32l4xx.h"
#include "stm32l4xx_hal_flash.h"
#include <aos/aos.h>
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

/* Private variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

static uint32_t FLASH_GetPage(uint32_t Addr)
{
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
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

  /* L4 ROM memory map, with 2 banks split into 2kBytes pages.
   * WARN: ABW. If the passed address and size are not page-aligned,
   * the start of the first page and the end of the last page are erased anyway.
   * After erase, the flash is left in unlocked state.
   */
  EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInit.Page = FLASH_GetPage(address);
  EraseInit.NbPages = FLASH_GetPage(address + len_bytes - 1) - EraseInit.Page + 1;
    
  HAL_FLASH_Unlock();
    
    if (HAL_FLASHEx_Erase(&EraseInit, &PageError) == HAL_OK)
    {
      rc = 0;
    }
    else
    {
      printf("Error erasing at 0x%08lx\n", address);
    }
  return rc;
}

/**
  * @brief  Write to FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed: Must be 4 byte aligned.
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
  
  for (i = 0; i < len_bytes; i += 8)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
        address + i,
        *(pData + (i/8) )) != HAL_OK)
    {
      break;
    }
  }
  /* Memory check */
  for (i = 0; i < len_bytes; i += 4)
  {
    uint32_t *dst = (uint32_t *)(address + i);
    uint32_t *src = ((uint32_t *) pData) + (i/4);
      
    if ( *dst != *src )
    {
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

/**
  * @brief  Update a chunk of the FLASH memory.
  * @note   The FLASH chunk must no cross a FLASH bank boundary.
  * @note   The source and destination buffers have no specific alignment constraints.
  * @param  In: dst_addr    Destination address in the FLASH memory.
  * @param  In: data        Source address. 
  * @param  In: size        Number of bytes to update.
  * @retval  0:  Success.
  *         <0:  Failure.
  */
int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size)
{
  int ret = 0;
  int remaining = size;
  uint8_t * src_addr = (uint8_t *) data;

  uint64_t * page_cache = NULL;
  page_cache = (uint64_t *)aos_malloc(FLASH_PAGE_SIZE);
  if (page_cache == NULL)
    return -1;
  memset(page_cache, 0, FLASH_PAGE_SIZE);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

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
    if (ret != 0)
    {
      printf("Error erasing at 0x%08lx\n", fl_addr);
    }
    else
    {
      ret = FLASH_write_at(fl_addr, page_cache, FLASH_PAGE_SIZE);
      if(ret != 0)
      {
        printf("Error writing %lu bytes at 0x%08lx\n", FLASH_PAGE_SIZE, fl_addr);
      }
      else
      {
        dst_addr += len;
        src_addr += len;
        remaining -= len;
      }
    }
  } while ((ret == 0) && (remaining > 0));
  
  aos_free(page_cache);
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


    for (i = 0; i < len_bytes; i += 4)
    {
        *(dst + i/4) = *(src++);
    }

    ret = 0;
    return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

