/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32f4xx.h"

#include "aos/hal/flash.h"

#include "k_api.h"

#include "stm32f4xx_hal_flash.h"

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */

#define ST_F4_SECTOR_23 (defined(STM32F427xx) || defined(STM32F437xx) || \
                         defined(STM32F429xx) || defined(STM32F439xx) || \
                         defined(STM32F469xx) || defined(STM32F479xx))

#define ST_F4_SECTOR_15 (defined(STM32F413xx) || defined(STM32F423xx) || \
                         ST_F4_SECTOR_23)

#define ST_F4_SECTOR_11 (defined(STM32F405xx) || defined(STM32F415xx) || \
                         defined(STM32F407xx) || defined(STM32F417xx) || \
                         defined(STM32F412Zx) || defined(STM32F412Vx) || \
                         defined(STM32F412Rx) || defined(STM32F412Cx) || \
                         ST_F4_SECTOR_15)

#define ST_F4_SECTOR_7 (defined(STM32F401xE) || defined(STM32F411xE) || \
                        defined(STM32F446xx) || ST_F4_SECTOR_11)

#define ST_F4_SECTOR_5 (defined(STM32F401xC) || ST_F4_SECTOR_7)

#define ST_F4_SECTOR_4 (defined(STM32F410Tx) || defined(STM32F410Cx) || \
                        defined(STM32F410Rx) || ST_F4_SECTOR_5)

extern const hal_logic_partition_t hal_partitions[];

static uint32_t GetSector(uint32_t Address);
int FLASH_read_at(uint32_t address, uint8_t *pData, uint32_t len_bytes);


hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf , uint32_t buf_size)
{
    int32_t ret=0;
    uint32_t start_addr;
    uint32_t byte_count=0;//count the number of programmed bytes
    uint8_t* src_data_p;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    start_addr = partition_info->partition_start_addr + *poff;
    if((*poff+buf_size) > partition_info->partition_length)
        return -1;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    src_data_p = (uint8_t*)buf;
    while(byte_count<buf_size)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,start_addr+byte_count,*(src_data_p++))!=HAL_OK)
        {
            ret =-1;
            break;
        }
        byte_count++;
    }
    *poff+=byte_count;
    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    return ret;
}


int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
    int32_t ret=0;
    hal_logic_partition_t *partition_info;
    uint32_t start_addr;
    uint32_t i=0;

    partition_info = hal_flash_get_info( pno );
    if (poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length) 
    {
        return -1;
    }

    start_addr = partition_info->partition_start_addr + *poff;	
    FLASH_read_at(start_addr,(uint8_t*)buf,buf_size);

    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    int32_t ret=0;
    hal_logic_partition_t *partition_info;
    uint32_t FirstSector = 0, NbOfSectors = 0;
    uint32_t SECTORError = 0;
    FLASH_EraseInitTypeDef EraseInit;

    partition_info = hal_flash_get_info( pno );
    if (size + off_set > partition_info->partition_length) {
        return -1;
    }

    FirstSector = GetSector(partition_info->partition_start_addr+off_set);
    NbOfSectors = GetSector(partition_info->partition_start_addr+off_set + size -1)-FirstSector+1;

    EraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;//require Device operating range: 2.7V to 3.6V 
    EraseInit.Sector =FirstSector;
    EraseInit.NbSectors =NbOfSectors;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    if(HAL_FLASHEx_Erase(&EraseInit, &SECTORError) != HAL_OK)
    {
        ret =-1;
    }

     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return ret;
}


/**
  * @brief  Read from FLASH memory.
  * @param  In: address     Destination address.
  * @param  In: pData       Data to be programmed: Must be 8 byte aligned.
  * @param  In: len_bytes   Number of bytes to be programmed.
  * @retval  0: Success.
            -1: Failure.
  */
int FLASH_read_at(uint32_t address, uint8_t *pData, uint32_t len_bytes)
{
    uint32_t i;
    int ret = -1;
    uint8_t *src = (uint8_t *)(address);
    uint8_t *dst = (uint8_t *)(pData);

    for (i = 0; i < len_bytes; i++)
    {
        *(dst++) = *(src++);
    }

    ret = 0;
    return ret;
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

#if ST_F4_SECTOR_4

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }

#endif

#if ST_F4_SECTOR_5

  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }

#endif

#if ST_F4_SECTOR_7

  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }

#endif

#if ST_F4_SECTOR_11

  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;
  }
#endif

#if ST_F4_SECTOR_15

  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;
  }
#endif

#if ST_F4_SECTOR_23

  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;
  }
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23) */
  {
    sector = FLASH_SECTOR_23;
  }

#endif

  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
/*
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
     (Sector == FLASH_SECTOR_3) || (Sector == FLASH_SECTOR_12) || (Sector == FLASH_SECTOR_13) ||\
     (Sector == FLASH_SECTOR_14) || (Sector == FLASH_SECTOR_15))
  {
    sectorsize = 16 * 1024;
  }
  else if((Sector == FLASH_SECTOR_4) || (Sector == FLASH_SECTOR_16))
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}
*/
#endif
