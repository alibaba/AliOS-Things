/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/flash.h"
#include "board.h"

#define ROUND_DOWN(a,b) 	(((a) / (b)) * (b))
#define MIN(a,b)        	(((a) < (b)) ? (a) : (b))
#define FLASH_PAGE_SIZE		FMC_FLASH_PAGE_SIZE

void platform_flash_lock(void)
{
  SYS_UnlockReg();                   /* Unlock register lock protect */
	FMC_DISABLE_AP_UPDATE();			/* Disable FMC ISP function */
	FMC_Close();									/* Disable APROM erase/program */
  SYS_LockReg();                   /* Unlock register lock protect */
}

void platform_flash_unlock(void)
{
  SYS_UnlockReg();                   /* Unlock register lock protect */	
	FMC_Open();										/* Enable FMC ISP function */
  FMC_ENABLE_AP_UPDATE();				/* Enable APROM erase/program */
}

static int32_t platform_flash_erase(uint32_t addr_start, uint32_t lengthInBytes)
{
	int ret = -1;
	int i=0;
	int page_num;
	
	//printf ("[%s]addr_start=%08x, size=%d\r\n", __func__, addr_start, lengthInBytes );
	
	if ( (addr_start%FMC_FLASH_PAGE_SIZE) != 0 ) //must be a 4k-aligned address
		goto exit_platform_flash_erase;
	
	page_num = ( (lengthInBytes-1)/FMC_FLASH_PAGE_SIZE ) +1;
	
	if ( (lengthInBytes%FMC_FLASH_PAGE_SIZE) != 0 ) //must be a 4k-aligned size
		printf ("[%s]lengthInBytes is not multiple of 8. will erase-%d pages\r\n", __func__, page_num );


	for ( i=0; i<page_num; i++ )
	 if (	(ret=FMC_Erase ( addr_start+i*FMC_FLASH_PAGE_SIZE )) < 0 )
		 break;
	 
	if ( i != page_num )
		goto exit_platform_flash_erase;		
	
	return 0;

exit_platform_flash_erase:
	
	return ret;	
}

/**
  * @brief   Program Multi-Word data into specified address of flash.
  * @param[in]  u32Addr    Start flash address in APROM where the data chunk to be programmed into.
  *                        This address must be 8-bytes aligned to flash address.
  * @param[in]  pu32Buf    Buffer that carry the data chunk.
  * @param[in]  u32Len     Length of the data chunk in bytes.
  * @retval   >=0  Number of data bytes were programmed.
  * @return   -1   Invalid address.
  */
static int32_t platform_flash_write_8bytes(uint32_t addr_start, uint32_t u32data0, uint32_t u32data1)
{
	//if ( (u32data0!=0xffffffff) || (u32data1!=0xffffffff) )
	//	printf ( "@@@@ [%s] %08x%08x@%08x\r\n", __func__, u32data1, u32data0, addr_start );
	
	return FMC_Write8Bytes((uint32_t)addr_start, u32data0, u32data1);
}

static int32_t platform_flash_write_4bytes(uint32_t addr_start, uint32_t u32data)
{
	//printf ( "@@@@ [%s] %08x@%08x\r\n", __func__, u32data, addr_start );	
	FMC_Write((uint32_t)addr_start, u32data);
}

static int32_t platform_flash_read(uint32_t* addr_start, const void *in_buf, uint32_t in_buf_len)
{
	
	//printf ("[%s]addr_start=%08x, pbuf=%08x size=%d\r\n", __func__, addr_start, in_buf, in_buf_len );
	
	if ( !addr_start  || !in_buf || !in_buf_len )
		goto exit_platform_flash_read;

	memcpy(in_buf, addr_start, in_buf_len);

	return in_buf_len;

exit_platform_flash_read:
	return -1;
}

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
    return platform_flash_erase(address, len_bytes);
}


int FLASH_WORD_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int remaining = size;
    int ret = 0;
    uint8_t *src_addr = (uint8_t *) data;
    uint8_t *debug_dst_addr = (uint8_t *) dst_addr;
    uint32_t fl_addr = 0;
    int fl_offset = 0;
    int len = 0;
    uint8_t word_cache[4] = {0};
	int i=0;

    do {
		fl_addr = ROUND_DOWN(dst_addr, 4);
        fl_offset = dst_addr - fl_addr;
        len = MIN(4 - fl_offset, remaining);

        /* Load from the flash into the cache */
        memcpy((void*)&word_cache[0], (void *) fl_addr, 4);
        /* Update the cache from the source */
        memcpy((void*)&word_cache[fl_offset], src_addr, len);
        /* Erase the page, and write the cache */

		platform_flash_write_4bytes(fl_addr, *((uint32_t*)&word_cache[0]));

		if ( *((uint32_t*)fl_addr) != *((uint32_t*)&word_cache[0]) ) {
			printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) fl_addr, *((uint32_t*)fl_addr), *((uint32_t*)&word_cache[0]) );
			ret = -1; 
			break;
		}
		
        dst_addr += len;
        src_addr += len;
        remaining -= len;
        
    } while ((remaining > 0));

    return ret ;
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
		uint32_t * psU32Data;
	
#ifndef CODE_UNDER_FIREWALL
    /* irq already mask under firewall */
    __disable_irq();
#endif

	if ( len_bytes < 4 )	//Update word
	{
		FLASH_WORD_update(address, (const void *)pData, len_bytes);				
		ret = 0;
	} else {

			for (i = 0; i < len_bytes; i += 4) {
				psU32Data = (uint32_t*)pData;
				platform_flash_write_4bytes ( address + i, psU32Data[i/4] );
			}
		/* Memory check */
		for (i = 0; i < len_bytes; i += 4) {
			uint32_t *dst = (uint32_t *)(address + i);
			uint32_t *src = ((uint32_t *) pData) + (i / 4);

			if ( *dst != *src ) {
	#ifndef CODE_UNDER_FIREWALL
				printf("Write failed @0x%08lx, read value=0x%08lx, expected=0x%08lx\n", (uint32_t) dst, *dst, *src);
	#endif
				//break;
			}
			ret = 0;
		}
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
    uint32_t *src = (uint32_t *)address;
    uint32_t *dst = (uint32_t *)pData;

    //printf("Read @0x%08lx to 0x%08lx, expected=0x%08lx\n", src, dst);

    for (i = 0; i < len_bytes; i += 4) {
        *(dst + i / 4) = *(src++);
    }

    return 0;
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

extern const hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

static void hal_flash_print_info(hal_partition_t in_partition)
{
	printf ("**** [%s]owner=%d (%s) @%08x, %d - %x\r\n", __func__, \
					hal_partitions[ in_partition ].partition_owner,
					hal_partitions[ in_partition ].partition_description,
					hal_partitions[ in_partition ].partition_start_addr,
					hal_partitions[ in_partition ].partition_length,
					hal_partitions[ in_partition ].partition_options );
}
/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *p_logic_partition;
	//hal_flash_print_info(in_partition);
    if (in_partition >= HAL_FLASH_NONE || partition == NULL) {
        return -1;
    }

    p_logic_partition = &hal_partitions[ in_partition ];
    memcpy(partition, p_logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int ret = 0;
    uint32_t start_addr;
    uint32_t erase_size;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;

	//printf ("[%s]off=%d, size=%d\r\n", __func__, off_set , size );

    platform_flash_unlock();

    real_pno = in_partition;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get( real_pno, p_partition_info );
    if ( ret != 0 || size + off_set > p_partition_info->partition_length) {
        ret = -1;
        goto exit_hal_flash_erase;
    }

    start_addr = ROUND_DOWN((p_partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);
    erase_size = p_partition_info->partition_start_addr + off_set - start_addr + size;

    ret = FLASH_erase_at(start_addr, erase_size);
    if (ret != 0) {
        printf("FLASH_erase_at return failed\n");
        ret = -1;
        goto exit_hal_flash_erase;
    }

exit_hal_flash_erase:
    platform_flash_lock();
    return ret;
}


/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    int ret = 0;
    int NeedUpdate=0;
    
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;

	//printf ("[%s]off=%d, pbuf=%08x size=%d\r\n", __func__, *off_set ,in_buf, in_buf_len );

    platform_flash_unlock();

    real_pno = in_partition;
    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get( real_pno, p_partition_info );
    start_addr = p_partition_info->partition_start_addr + *off_set;
    
    //NeedUpdate = (start_addr%4) + (in_buf_len%4) + (in_buf_len/FLASH_PAGE_SIZE) ;
    NeedUpdate = (in_buf_len/FLASH_PAGE_SIZE) ;
    
    #if 0
    printf("[%s]%d, %s(0x%08x) %s(%d) %s(%d)\n", __func__, NeedUpdate, \
					(start_addr%4)?"Non-Wordalign":"WordAlign", start_addr, \
					(in_buf_len%4)?"Non-WordLength":"WordLength", in_buf_len, \
					(in_buf_len/FLASH_PAGE_SIZE)?"OverAPageSize":"NotOverAPageSize", in_buf_len);
    #endif
    
    if ( NeedUpdate )
	{
		if (0 != FLASH_update(start_addr, in_buf, in_buf_len)) 
		{
			printf("FLASH_update page failed!\n");
			ret = -1;
			goto exit_hal_flash_write;
		}
	}
	else if (0 != FLASH_write_at(start_addr, in_buf, in_buf_len) ) 
	{
        printf("FLASH_update word failed!\n");
        ret = -1;
        goto exit_hal_flash_write;
    }
    
    *off_set += in_buf_len;

exit_hal_flash_write:

    platform_flash_lock();
    return ret;
}

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
	//printf ("[%s]off=%d, pbuf=%08x size=%d\r\n", __func__, *off_set ,in_buf, in_buf_len );

	if ( hal_flash_erase(in_partition, *off_set, in_buf_len) < 0 )
		return HAL_ERROR;
	else if ( hal_flash_write(in_partition, off_set, in_buf, in_buf_len) < 0 )
		return HAL_ERROR;
	
	return HAL_OK;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    int32_t ret;
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;
    hal_partition_t real_pno;
 
    real_pno = in_partition;
    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    ret = hal_flash_info_get( real_pno, p_partition_info );
	//printf ("[%s] off_set=%x, out_buf=%x %d>%d\r\n", __func__, off_set, out_buf, (*off_set + in_buf_len), partition_info->partition_length );

    if (ret != 0 || off_set == NULL || out_buf == NULL || *off_set + in_buf_len > p_partition_info->partition_length) {
        return -1;
    }

platform_flash_unlock();
    start_addr = p_partition_info->partition_start_addr + *off_set;
    FLASH_read_at(start_addr, out_buf, in_buf_len);
    *off_set += in_buf_len;
platform_flash_lock();

    return 0;
}

/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
	return HAL_OK;
}

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
	return HAL_OK;
}

