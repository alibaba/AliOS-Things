#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash.h"

#define EN_DBG 0

#define KM_FLASH_ADDR_START 0x080FF000
#define KM_FLASH_ADDR_SIZE 0x1000
#define FLASH_BLOCK_LEN FLASH_PAGE_SIZE

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
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
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
    /* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
    /* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }

  return bank;
}

int32_t osa_flash_read(void *addr, void *out, size_t size)
{
#if EN_DBG
    printf("flash read addr 0x%08x size 0x%08x\n", (uint32_t)addr, size);
#endif
    memcpy(out, addr ,size);

#if EN_DBG
    printf("data 0x%02x 0x%02x 0x%02x 0x%02x\n",
    *((uint8_t *)out + 0), *((uint8_t *)out + 1),
    *((uint8_t *)out + 2), *((uint8_t *)out + 3));
#endif
    return 0;
}

int32_t osa_flash_write(void *addr, void *buf, size_t size)
{
    uint64_t *p = (uint64_t *)buf;
    uint32_t a = (uint32_t)addr;
    HAL_StatusTypeDef ret = HAL_OK;

    if (0 != ((uint32_t)addr % 8) || 0 != (size % 8)) {
        printf("bad param addr 0x%08x size 0x%08x\n",
                (unsigned int)addr , (unsigned int)size);
        return -1;
    }
#if EN_DBG
    printf("flash write addr 0x%08x size 0x%08x\n",
            (uint32_t)addr , (uint32_t)size);
#endif

    HAL_FLASH_Unlock();
    while ((size >= 8) && (ret == HAL_OK)) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)a, (uint64_t)(*p));
        if (0 != ret) {
            printf("write flash fail addr %08x data %llx ret 0x%08x\n",
                    (unsigned int)a, (uint64_t)(*p), (unsigned int)ret);
            return -1;
        }
#if EN_DBG
        uint8_t tmp_buf[8];
        int i;

        osa_flash_read((void *)a , tmp_buf, 8);
        if (0 != memcmp(tmp_buf, p, 8)) {
            printf("write than read not match \n");

            for (i = 0; i < 8; ++i) {
                printf("write buf 0x%02x read data 0x%02x\n", *((uint8_t *)p + i), tmp_buf[i]);
            }
            return -1;
        }
        #if 0
        for (i = 0; i < 8; ++i) {
            printf("write buf 0x%02x read data 0x%02x\n", *((uint8_t *)p + i), tmp_buf[i]);
        }
        #endif
#endif
        p = (uint64_t *)((uint32_t)p + 8);
        a += 8;
        size -= 8;
    }
    HAL_FLASH_Lock();

    return 0;
}

int32_t osa_flash_erase(void *addr, size_t size)
{
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t cur_addr = (uint32_t)addr;
    size_t cur_size = size;

    HAL_FLASH_Unlock();
    if (0 != (size % FLASH_BLOCK_LEN) || 0 != (cur_addr % FLASH_BLOCK_LEN)) {
        printf("bad param addr 0x%08x size 0x%08x\n",
                (unsigned int)addr, (unsigned int)size);
        return -1;
    }

    if (0 == size) {
        return 0;
    }
    while (cur_size > 0) {
        /* Fill EraseInit structure*/
        pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
        pEraseInit.Banks       = GetBank(cur_addr);
        pEraseInit.Page        = GetPage(cur_addr);
        pEraseInit.NbPages     = 1;
#if EN_DBG
        printf("flash erase page %d bank %d\n", GetPage(cur_addr), GetBank(cur_addr));
#endif
        if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK)
        {
            printf("flash erase fail\n");
            return -1;
        }
        cur_size -= FLASH_BLOCK_LEN;
        cur_addr -= FLASH_BLOCK_LEN;
    }
    HAL_FLASH_Lock();

    return 0;
}

int32_t getRDPLevel(uint32_t *RDPLevel)
{
  FLASH_OBProgramInitTypeDef sFlashOptionBytes;
  int32_t eRetStatus = 0;

   if (NULL == RDPLevel) {
       return -1;
   }
  /* Unlock the Flash to enable the flash control register access
*************/
  HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  /* Unlock the Options Bytes
*************************************************/
  HAL_FLASH_OB_Unlock();

  /* Get Option Bytes status for FLASH_BANK_1: WRP AREA_A  and PCRoP
**********/
  HAL_FLASHEx_OBGetConfig(&sFlashOptionBytes);

  *RDPLevel = sFlashOptionBytes.RDPLevel;

  /* Lock the Options Bytes
***************************************************/
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();

  return eRetStatus;
}

int32_t ConfigWRP (bool enable, bool reboot)
{
  int32_t eRetStatus = 0;
  uint32_t RDPlevel;
  uint32_t StartPage = 0, EndPage = 0, StartBank = 0, EndBank = 0;
  FLASH_OBProgramInitTypeDef psFlashOptionBytes, psFlashOptionBytes2;

  eRetStatus = getRDPLevel(&RDPlevel);
  if (0 != eRetStatus) {
      return -1;
  }

  if (OB_RDP_LEVEL_2 == RDPlevel) {
      return -1;
  }

  /* Get the number of the start and end pages */
  StartPage = GetPage(KM_FLASH_ADDR_START);
  EndPage   = GetPage(KM_FLASH_ADDR_START + KM_FLASH_ADDR_SIZE - 1);

  /* Get the bank of the start and end pages */
  StartBank = GetBank(KM_FLASH_ADDR_START);
  EndBank   = GetBank(KM_FLASH_ADDR_START + KM_FLASH_ADDR_SIZE - 1);

  if (StartBank != EndBank) {
      /* cross bank not support now */
      return -1;
  }

  /* WRP area is only on one bank */
  if (StartBank == FLASH_BANK_1)
  {
    psFlashOptionBytes.WRPArea  = OB_WRPAREA_BANK1_AREAA;
    psFlashOptionBytes2.WRPArea = OB_WRPAREA_BANK1_AREAB;
  }
  else
  {
    psFlashOptionBytes.WRPArea  = OB_WRPAREA_BANK2_AREAA;
    psFlashOptionBytes2.WRPArea = OB_WRPAREA_BANK2_AREAB;
  }

  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  HAL_FLASH_OB_Unlock();

  HAL_FLASHEx_OBGetConfig(&psFlashOptionBytes);
  HAL_FLASHEx_OBGetConfig(&psFlashOptionBytes2);
  if (true == enable) {
      /* Check if desired pages are not yet write protected ***********************/
      if ((psFlashOptionBytes.WRPStartOffset <= StartPage) && (psFlashOptionBytes.WRPEndOffset >= (StartPage - 1)))
      {
        /* Current area is adjacent to pages to be write protected */
        if (psFlashOptionBytes.WRPEndOffset < EndPage)
        {
          /* Current area will be extended to include the pages to be write protected */
          psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes.WRPEndOffset   = EndPage;
        }
      }
      else if ((psFlashOptionBytes.WRPStartOffset <= (EndPage + 1)) && (psFlashOptionBytes.WRPEndOffset >= EndPage))
      {
        /* Current area is adjacent to pages to be write protected */
        if (psFlashOptionBytes.WRPStartOffset > StartPage)
        {
          /* Current area will be extended to include the pages to be write protected */
          psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes.WRPStartOffset = StartPage;
        }
      }
      else if ((psFlashOptionBytes.WRPStartOffset > StartPage) && (psFlashOptionBytes.WRPEndOffset < EndPage))
      {
        /* Current area is included in pages to be write protected */
        psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes.WRPStartOffset = StartPage;
        psFlashOptionBytes.WRPEndOffset   = EndPage;
      }
      else if ((psFlashOptionBytes2.WRPStartOffset <= StartPage) && (psFlashOptionBytes2.WRPEndOffset >= (StartPage - 1)))
      {
        /* Current area is adjacent to pages to be write protected */
        if (psFlashOptionBytes2.WRPEndOffset < EndPage)
        {
          /* Current area will be extended to include the pages to be write protected */
          psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes2.WRPEndOffset   = EndPage;
        }
      }
      else if ((psFlashOptionBytes2.WRPStartOffset <= (EndPage + 1)) && (psFlashOptionBytes2.WRPEndOffset >= EndPage))
      {
        /* Current area is adjacent to pages to be write protected */
        if (psFlashOptionBytes2.WRPStartOffset > StartPage)
        {
          /* Current area will be extended to include the pages to be write protected */
          psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes2.WRPStartOffset = StartPage;
        }
      }
      else if ((psFlashOptionBytes2.WRPStartOffset > StartPage) && (psFlashOptionBytes2.WRPEndOffset < EndPage))
      {
        /* Current area is included in pages to be write protected */
        psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes2.WRPStartOffset = StartPage;
        psFlashOptionBytes2.WRPEndOffset   = EndPage;
      }
      else if (psFlashOptionBytes.WRPStartOffset > psFlashOptionBytes.WRPEndOffset)
      {
        /* Current area is not used => it will be configured to protect the pages */
        psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes.WRPStartOffset = StartPage;
        psFlashOptionBytes.WRPEndOffset   = EndPage;
      }
      else if (psFlashOptionBytes2.WRPStartOffset > psFlashOptionBytes2.WRPEndOffset)
      {
        /* Current area is not used => it will be configured to protect the pages */
        psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes2.WRPStartOffset = StartPage;
        psFlashOptionBytes2.WRPEndOffset   = EndPage;
      }
      else
      {
        /* No more area available to protect the pages */
        /* => Error : not possible to activate the pages indicated */
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        eRetStatus = -1;
      }
  } else {
      /* disable */
      /* Check if desired pages are already write protected ***********************/
      if ((psFlashOptionBytes.WRPStartOffset == StartPage) && (psFlashOptionBytes.WRPEndOffset == EndPage))
      {
        /* Current area correspond to the area to disable */
        psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes.WRPStartOffset = 0xFF;
        psFlashOptionBytes.WRPEndOffset   = 0;
      }
      else if ((psFlashOptionBytes.WRPStartOffset == StartPage) && (psFlashOptionBytes.WRPEndOffset > EndPage))
      {
        /* Current area is bigger than the area to disable : */
        /* - End of area is bigger than the last page to un-protect */
        psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes.WRPStartOffset = EndPage + 1;
      }
      else if ((psFlashOptionBytes.WRPStartOffset < StartPage) && (psFlashOptionBytes.WRPEndOffset == EndPage))
      {
        /* Current area is bigger than the area to disable : */
        /* - Start of area is lower than the first page to un-protect */
        psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes.WRPEndOffset   = StartPage - 1;
      }
      else if ((psFlashOptionBytes.WRPStartOffset < StartPage) && (psFlashOptionBytes.WRPEndOffset > EndPage))
      {
        /* Current area is bigger than the area to disable */
        /* - Start of area is lower than the first page to un-protect */
        /* - End of area is bigger than the last page to un-protect */
        if (psFlashOptionBytes2.WRPStartOffset > psFlashOptionBytes2.WRPEndOffset)
        {
          /* Second area of the bank can be used */
          psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes2.WRPStartOffset = EndPage + 1;
          psFlashOptionBytes2.WRPEndOffset   = psFlashOptionBytes.WRPEndOffset;

          psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes.WRPEndOffset   = StartPage - 1;
        }
        else
        {
          /* Second area of the bank already used for WRP */
          /* => Error : not possible to deactivate only the pages indicated */
            HAL_FLASH_OB_Lock();
            HAL_FLASH_Lock();
            eRetStatus = -1;
        }
      }
      else if ((psFlashOptionBytes2.WRPStartOffset == StartPage) && (psFlashOptionBytes2.WRPEndOffset == EndPage))
      {
        /* Current area correspond to the area to disable */
        psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes2.WRPStartOffset = 0xFF;
        psFlashOptionBytes2.WRPEndOffset   = 0;
      }
      else if ((psFlashOptionBytes2.WRPStartOffset == StartPage) && (psFlashOptionBytes2.WRPEndOffset > EndPage))
      {
        /* Current area is bigger than the area to disable : */
        /* - End of area is bigger than the last page to un-protect */
        psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes2.WRPStartOffset = EndPage + 1;
      }
      else if ((psFlashOptionBytes2.WRPStartOffset < StartPage) && (psFlashOptionBytes2.WRPEndOffset == EndPage))
      {
        /* Current area is bigger than the area to disable : */
        /* - Start of area is lower than the first page to un-protect */
        psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
        psFlashOptionBytes2.WRPEndOffset   = StartPage - 1;
      }
      else if ((psFlashOptionBytes2.WRPStartOffset < StartPage) && (psFlashOptionBytes2.WRPEndOffset > EndPage))
      {
        /* Current area is bigger than the area to disable */
        /* - Start of area is lower than the first page to un-protect */
        /* - End of area is bigger than the last page to un-protect */
        if (psFlashOptionBytes.WRPStartOffset > psFlashOptionBytes.WRPEndOffset)
        {
          /* Second area of the bank can be used */
          psFlashOptionBytes.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes.WRPStartOffset = EndPage + 1;
          psFlashOptionBytes.WRPEndOffset   = psFlashOptionBytes2.WRPEndOffset;

          psFlashOptionBytes2.OptionType     = OPTIONBYTE_WRP;
          psFlashOptionBytes2.WRPEndOffset   = StartPage - 1;
        }
        else
        {
          /* Second area of the bank already used for WRP */
          /* => Error : not possible to deactivate only the pages indicated */
            HAL_FLASH_OB_Lock();
            HAL_FLASH_Lock();
            eRetStatus = -1;
        }
      }
  }

  /* Configure write protected pages */
  if (psFlashOptionBytes.OptionType == OPTIONBYTE_WRP)
  {
    if(HAL_FLASHEx_OBProgram(&psFlashOptionBytes) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        eRetStatus = -1;
    }
  }

  if (psFlashOptionBytes2.OptionType == OPTIONBYTE_WRP)
  {
    if(HAL_FLASHEx_OBProgram(&psFlashOptionBytes2) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        eRetStatus = -1;
    }
  }

  /* Generate System Reset to load the new option byte values ***************/
  if (((psFlashOptionBytes.OptionType == OPTIONBYTE_WRP) || (psFlashOptionBytes2.OptionType == OPTIONBYTE_WRP)) && reboot)
  {
    HAL_FLASH_OB_Launch();
  }

  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
  return eRetStatus;
}

int32_t CleanProtectionWRP (void)
{
  int32_t eRetStatus = 0;
  uint32_t RDPlevel;
  FLASH_OBProgramInitTypeDef psFlashOptionBytes;

  eRetStatus = getRDPLevel(&RDPlevel);
  if (0 != eRetStatus) {
      return -1;
  }

  if (OB_RDP_LEVEL_2 == RDPlevel) {
      return -1;
  }

  psFlashOptionBytes.OptionType        = OPTIONBYTE_WRP;
  psFlashOptionBytes.WRPArea           = 0;//SFU_HAL_IF_PROTECT_WRP_AREA_1;
  psFlashOptionBytes.WRPStartOffset    = 0;
  psFlashOptionBytes.WRPEndOffset      = 0;

  if (HAL_FLASHEx_OBProgram(&psFlashOptionBytes) != HAL_OK) {
    eRetStatus = -1;
  }

  return eRetStatus;
}
