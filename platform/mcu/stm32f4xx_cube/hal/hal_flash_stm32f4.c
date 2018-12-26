/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/**
 ******************************************************************************
 * @file    hal_flash_stm32f4.c
 * @author  MCU China FAE team
 * @version 1.0
 * @date    26/12/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_FLASH_MODULE_ENABLED

#include <k_api.h>
#include "hal/soc/soc.h"
#include "stm32f4xx_hal_flash.h"
#include <aos/kernel.h>

extern const hal_logic_partition_t hal_partitions[];

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define ROUND_DOWN(a, b) (((a) / (b)) * (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define BLOCK_HEAD_SIZE 8
#define BLOCK_MAGIC_STRING "STM32F4X"

/* Private variables
 * ----------------------------------------------------------*/
aos_mutex_t hal_flash_mutex;
static int32_t hal_mutex_create = 0;

/* F429 dual bank  2Mbyte organization. */
const uint32_t l_sector_map[] = {
    0x08000000, 0x08004000, 0x08008000,
    0x0800C000, // 16 kbytes sectors
    0x08010000, // 64 kbytes sector
    0x08020000, 0x08040000, 0x08060000,
    0x08080000, // 128 kbytes sectors...
    0x080A0000, 0x080C0000, 0x080E0000, 0x08100000, 0x08104000, 0x08108000,
    0x0810C000, // 16 kbytes sectors
    0x08110000, // 64 kbytes sector
    0x08120000, 0x08140000, 0x08160000,
    0x08180000, // 128 kbytes sectors...
    0x081A0000, 0x081C0000, 0x081E0000,
    0x08200000 // The boundary*/
};

/* Private function prototypes -----------------------------------------------*/
static int32_t GetSector(uint32_t Address);
static int32_t GetBlockBase(hal_logic_partition_t *partition_info,
                            uint32_t *cur, int32_t *next);
/* Functions Definition ------------------------------------------------------*/

/**
 * @brief   Switch to the other bank than the one which is currently used.
 * @note
 * @param   None
 * @retval  0: Success.
 *         <0: Failure.
 */
int FLASH_set_boot_bank() {
  int rc = 0;
  int ret = HAL_OK;
  FLASH_AdvOBProgramInitTypeDef AdvOBInit;

  /* Set BFB2 bit to enable boot from Flash Bank2 */
  /* Allow Access to Flash control registers and user Flash */
  HAL_FLASH_Unlock();

  /* Allow Access to option bytes sector */
  HAL_FLASH_OB_Unlock();

  AdvOBInit.OptionType = OBEX_BOOTCONFIG;
  HAL_FLASHEx_AdvOBGetConfig(&AdvOBInit);

  if (((AdvOBInit.BootConfig) & (FLASH_OPTCR_BFB2)) == FLASH_OPTCR_BFB2) {
    printf("Switch to Bank1\n");
    AdvOBInit.BootConfig = OB_DUAL_BOOT_DISABLE;
  } else {
    printf("Switch to Bank2\n");
    AdvOBInit.BootConfig = OB_DUAL_BOOT_ENABLE;
  }

  ret = HAL_FLASHEx_AdvOBProgram(&AdvOBInit);

  if (ret != HAL_OK) {
    printf("!BANK Switch failed\n");
    rc = -1;
  }

  ret = HAL_FLASH_OB_Launch();
  if (ret != HAL_OK) {
    printf("!BANK Switch failed\n");
    rc = -1;
  }
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
  HAL_Delay(50);

  return rc;
}

int FLASH_bank2_enabled() {
  FLASH_AdvOBProgramInitTypeDef AdvOBInit;

  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  AdvOBInit.OptionType = OBEX_BOOTCONFIG;
  HAL_FLASHEx_AdvOBGetConfig(&AdvOBInit);

  if (((AdvOBInit.BootConfig) & (FLASH_OPTCR_BFB2)) == FLASH_OPTCR_BFB2) {
    return 1;
  } else {
    return 0;
  }
}
/**
  * @brief  Erase FLASH memory sector(s) at address.
  * @param  In: address     Start address to erase from.
  * @param  In: len_bytes   Length to be erased.
  * @retval  0:  Success.
            -1:  Failure.
  */
int FLASH_unlock_erase(uint32_t address, uint32_t len_bytes) {
  int rc = -1;
  uint32_t SectorError = 0;
  FLASH_EraseInitTypeDef EraseInit;

  EraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInit.Sector = GetSector(address);
  EraseInit.NbSectors =
      GetSector(address + len_bytes - 1) - EraseInit.Sector + 1;

  HAL_FLASH_Unlock();
  if (HAL_FLASHEx_Erase(&EraseInit, &SectorError) == HAL_OK) {
    rc = 0;
  } else {
    printf("Error %lu erasing at 0x%08lx\n", SectorError, address);
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
int FLASH_write_at(uint32_t address, uint32_t *pData, uint32_t len_bytes) {
  int i;
  int ret = -1;
  uint32_t TypeProgram = 0;
  uint32_t step = 0;

  /*Without external Vpp it should NOT be 8 bytes parallelism*/
  if (len_bytes % 4 == 0) {
    TypeProgram = FLASH_TYPEPROGRAM_WORD;
    step = 4;
  } else if (len_bytes % 2 == 0) {
    TypeProgram = FLASH_TYPEPROGRAM_HALFWORD;
    step = 2;
  } else {
    TypeProgram = FLASH_TYPEPROGRAM_BYTE;
    step = 1;
  }

  for (i = 0; i < len_bytes; i += step) {
    __disable_irq();
    ret = HAL_FLASH_Program(TypeProgram, address + i, *(pData + (i / step)));
    __enable_irq();
    if (ret != HAL_OK) {
      break;
    }
  }

  /* Memory check */
  if (memcmp((uint8_t *)address, (uint8_t *)pData, len_bytes) != 0) {
    ret = -1;
  } else {
    ret = 0;
  }

  return ret;
}

/**
 * @brief  Update a chunk of the FLASH memory.
 * @note   The FLASH chunk must no cross a FLASH bank boundary.
 * @note   The source and destination buffers have no specific alignment
 * constraints.
 * @param  In: dst_addr    Destination address in the FLASH memory.
 * @param  In: data        Source address.
 * @param  In: size        Number of bytes to update.
 * @retval  0:  Success.
 *         <0:  Failure (at malloc, erase, or write).
 */
int FLASH_update(hal_logic_partition_t *partition_info, uint32_t off,
                 const void *data, uint32_t size) {
  int rc = -1;
  int ret = 0;
  int sector_cache_size = 0;
  int remaining = size;
  uint8_t *src_addr = (uint8_t *)data;
  uint8_t *sector_cache = NULL;
  uint8_t *sector_cache_buffer = NULL;
  uint32_t cur;
  uint32_t next;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |
                         FLASH_FLAG_PGSERR);

  /* Compute the size to be updated. */
  sector_cache_size = partition_info->partition_length + BLOCK_HEAD_SIZE;

  /*Get current block and next available one*/
  GetBlockBase(partition_info, &cur, &next);
  /*already 8 bytes aligned*/
  sector_cache_buffer = (uint8_t *)malloc(sector_cache_size + 8);

  if (sector_cache_buffer != NULL) {
    sector_cache = (uint8_t *)((uint32_t)sector_cache_buffer & ~0x7);
    /* Load from the flash into the cache */
    memcpy(sector_cache, (uint8_t *)cur, sector_cache_size);

    /* Update the cache from the source */
    memcpy((uint8_t *)sector_cache + BLOCK_HEAD_SIZE + off, src_addr, size);
    memcpy((void *)sector_cache, BLOCK_MAGIC_STRING, BLOCK_HEAD_SIZE);

    int i = 0;
    do {
      i++;
      if (next != NULL && ret == 0) {
        ret = FLASH_write_at(next, sector_cache, sector_cache_size);
        if (ret == 0) {
          break;
        }
      }

      if (next == NULL || ret != 0) {
        /* Erase the page, and write the cache */
        ret = FLASH_unlock_erase(partition_info->partition_start_addr,
                                 sector_cache_size);
        if (ret != 0) {
          printf("Error erasing at 0x%x\n",
                 partition_info->partition_start_addr);
        }
        /*After erasing the cursor will be start of partition*/
        next = partition_info->partition_start_addr;
      }
    } while (i < 3);

    if (ret == 0) {
      rc = 0;
    } else {
      printf("Flash update at %x ERROR\n", next);
    }

    free(sector_cache_buffer);
  }
  return rc;
}

/*****************************************************************/
/**
  * @brief  Read from FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be read.
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_read_at(uint32_t address, uint64_t *pData, uint32_t len_bytes) {
  int i;
  int ret = -1;

  /*Flash reading can be in byte*/
  memcpy((uint8_t *)pData, (uint8_t *)address, len_bytes);

  ret = 0;
  return ret;
}

/**
 * @brief  Gets the sector number of a given address.
 * @param  In: address
 * @retval >=0 Sector number.
 *         -1  Error: Invalid address.
 */
static int32_t GetSector(uint32_t address) {
  int32_t sector = -1;
  int i = 0;

  if ((l_sector_map[0] <= address) &&
      (address < l_sector_map[sizeof(l_sector_map) / sizeof(uint32_t) -
                              1])) { /* The address is within the range of the
                                        internal flash. */
    for (i = 0; i < (sizeof(l_sector_map) / sizeof(uint32_t) - 1); i++) {
      if (address < l_sector_map[i + 1]) {
        /* Matching sector found. */
        sector = i;
        break;
      }
    }
  }
  return sector;
}

static int32_t GetBlockBase(hal_logic_partition_t *partition_info,
                            uint32_t *cur, int32_t *next) {
  hal_partition_t real_pno;
  uint32_t sector;
  uint32_t i;
  uint32_t real_address;
  uint32_t total_blocks;
  uint32_t block_len;
  uint32_t block_blank;

  *cur = -1;

  /*By default it's the start of partition*/
  sector = GetSector(partition_info->partition_start_addr);
  block_len = partition_info->partition_length + BLOCK_HEAD_SIZE;
  block_blank = (l_sector_map[sector + 1] - l_sector_map[sector]) % block_len;

  /*Find the current used block*/
  for (i = l_sector_map[sector + 1] - block_blank; i >= l_sector_map[sector];
       i -= block_len) {
    if (memcmp((int32_t *)i, BLOCK_MAGIC_STRING, BLOCK_HEAD_SIZE) == 0) {
      *cur = i;
      break;
    }
  }

  if (next != NULL) {
    if (*cur != -1) {
      *next = i + block_len;
      if (*next == l_sector_map[sector + 1] - block_blank) {
        *next = NULL; /* to indicate one erase is necessary*/
      }
    } else {
      /*The first time writting*/
      *next = partition_info->partition_start_addr;
    }
  }

  if (*cur == -1) {
    /*The first time writting*/
    *cur = partition_info->partition_start_addr;
  }
  return 0;
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno) {
  hal_logic_partition_t *logic_partition;

  logic_partition = (hal_logic_partition_t *)&hal_partitions[pno];

  return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf,
                        uint32_t buf_size) {
  uint32_t start_addr;
  hal_logic_partition_t *partition_info;
  hal_partition_t real_pno = pno;
  int32_t ret;

  /*SWAP BANK*/
  if (FLASH_bank2_enabled()) /*BFB2 == 1*/
  {
    if (pno == HAL_PARTITION_OTA_TEMP) {
      real_pno = HAL_PARTITION_APPLICATION;
    }
    if (pno == HAL_PARTITION_PARAMETER_1) {
      real_pno = HAL_PARTITION_PARAMETER_3;
    }
    if (pno == HAL_PARTITION_PARAMETER_2) {
      real_pno = HAL_PARTITION_PARAMETER_4;
    }
    if (pno == HAL_PARTITION_PARAMETER_3) {
      real_pno = HAL_PARTITION_PARAMETER_1;
    }
    if (pno == HAL_PARTITION_PARAMETER_4) {
      real_pno = HAL_PARTITION_PARAMETER_2;
    }
  }

  /*It look KV read/write in multiple thread*/
  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    if (hal_mutex_create == 0) {
      ret = aos_mutex_new(&hal_flash_mutex);
      if (ret == -1) {
        printf("Flash mutex create failed\n");
      } else {
        hal_mutex_create = 1;
      }
    }
  }

  partition_info = hal_flash_get_info(real_pno);

  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    aos_mutex_lock(&hal_flash_mutex, AOS_WAIT_FOREVER);
  }
  /*OTA doesn't call this function,as it's too large*/
  if (0 != FLASH_update(partition_info, *poff, buf, buf_size)) {
    printf("FLASH_update at failed!\n");
  }

  *poff += buf_size;
  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    aos_mutex_free(&hal_flash_mutex);
  }
  return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf,
                       uint32_t buf_size) {
  uint32_t start_addr;
  hal_logic_partition_t *partition_info;
  hal_partition_t real_pno = pno;
  uint32_t cur;

  /*SWAP BANK*/
  if (FLASH_bank2_enabled()) /*BFB2 == 1*/
  {
    if (pno == HAL_PARTITION_OTA_TEMP) {
      real_pno = HAL_PARTITION_APPLICATION;
    }
    if (pno == HAL_PARTITION_PARAMETER_1) {
      real_pno = HAL_PARTITION_PARAMETER_3;
    }
    if (pno == HAL_PARTITION_PARAMETER_2) {
      real_pno = HAL_PARTITION_PARAMETER_4;
    }
    if (pno == HAL_PARTITION_PARAMETER_3) {
      real_pno = HAL_PARTITION_PARAMETER_1;
    }
    if (pno == HAL_PARTITION_PARAMETER_4) {
      real_pno = HAL_PARTITION_PARAMETER_2;
    }
  }

  partition_info = hal_flash_get_info(real_pno);

  if (poff == NULL || buf == NULL ||
      *poff + buf_size > partition_info->partition_length) {
    return -1;
  }
  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    GetBlockBase(partition_info, &cur, NULL);
    start_addr = cur + BLOCK_HEAD_SIZE + *poff;
  } else {
    start_addr = partition_info->partition_start_addr + *poff;
  }
  FLASH_read_at(start_addr, buf, buf_size);
  *poff += buf_size;

  return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size) {
  uint32_t start_addr;
  hal_logic_partition_t *partition_info;
  hal_partition_t real_pno = pno;
  int32_t toerase = 1;
  uint8_t *buf = NULL;

  int32_t ret;

  /*SWAP BANK*/
  if (FLASH_bank2_enabled()) /*BFB2 == 1*/
  {
    if (pno == HAL_PARTITION_OTA_TEMP) {
      real_pno = HAL_PARTITION_APPLICATION;
    }
    if (pno == HAL_PARTITION_PARAMETER_1) {
      real_pno = HAL_PARTITION_PARAMETER_3;
    }
    if (pno == HAL_PARTITION_PARAMETER_2) {
      real_pno = HAL_PARTITION_PARAMETER_4;
    }
    if (pno == HAL_PARTITION_PARAMETER_3) {
      real_pno = HAL_PARTITION_PARAMETER_1;
    }
    if (pno == HAL_PARTITION_PARAMETER_4) {
      real_pno = HAL_PARTITION_PARAMETER_2;
    }
  }

  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    if (hal_mutex_create == 0) {
      ret = aos_mutex_new(&hal_flash_mutex);
      if (ret == -1) {
        printf("Flash mutex create failed\n");
      } else {
        hal_mutex_create = 1;
      }
    }
  }

  partition_info = hal_flash_get_info(real_pno);
  if (size + off_set > partition_info->partition_length) {
    return -1;
  }

  aos_mutex_lock(&hal_flash_mutex, AOS_WAIT_FOREVER);
  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    if (memcmp((uint8_t *)partition_info->partition_start_addr,
               BLOCK_MAGIC_STRING, BLOCK_HEAD_SIZE) != 0) {
      toerase = 1;
    } else {
      toerase = 0;
      /*Still format KV sub block value*/
      buf = (uint8_t *)aos_malloc(size);
      if (buf != NULL) {
        memset(buf, 0xFF, size);
        if (0 != FLASH_update(partition_info, off_set, buf, size)) {
          printf("FLASH_update failed!\n");
        }
        aos_free(buf);
      }
    }
  }

  start_addr = partition_info->partition_start_addr;
  if (toerase == 1) {
    FLASH_unlock_erase(start_addr, size);
  }
  if (real_pno == HAL_PARTITION_PARAMETER_1 ||
      real_pno == HAL_PARTITION_PARAMETER_2 ||
      real_pno == HAL_PARTITION_PARAMETER_3 ||
      real_pno == HAL_PARTITION_PARAMETER_4) {
    aos_mutex_free(&hal_flash_mutex);
  }
  return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set,
                                uint32_t size) {
  return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set,
                             uint32_t size) {
  return 0;
}
#endif
