/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "hal_conf.h"
#include <k_api.h>
#include "aos/hal/flash.h"
#include "aos/errno.h"


#ifdef HAL_FLASH_MODULE_ENABLED

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
	hal_logic_partition_t *logic_partition;

	logic_partition = (hal_logic_partition_t *)&hal_logic_partition[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

	return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
#define	FLASH_BYTES_ALIGNED			16

	uint32_t start_addr = 0,odd = 0,phyAddrAlign = 0,memcpylen = 0,sizeLeft = in_buf_len;
	hal_logic_partition_t  partition_info;
	hal_logic_partition_t *p_partition_info;
	uint8_t destbuffer[FLASH_BYTES_ALIGNED]={0x00};
	const uint8_t *srcbuffer = (const uint8_t *)in_buf;
	
	if(sizeLeft == 0)
	{
		return EIO;
	}

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
	hal_flash_info_get( in_partition, p_partition_info );
	start_addr = p_partition_info->partition_start_addr + *off_set;
	
	/* Check if the startaddress is the page size aligned */
	if ((start_addr % FLASH_BYTES_ALIGNED) != 0)
	{
		odd = start_addr % FLASH_BYTES_ALIGNED;
		phyAddrAlign = (start_addr / FLASH_BYTES_ALIGNED) * FLASH_BYTES_ALIGNED;
		memcpylen = sizeLeft;

		/* Check if the area across pages. */
		if (odd + sizeLeft > FLASH_BYTES_ALIGNED)
		{
			memcpylen = FLASH_BYTES_ALIGNED - odd;
		}

		/* Need to read the page first*/
		memcpy((void*)destbuffer, (const void*)phyAddrAlign , FLASH_BYTES_ALIGNED);

		/* Change the data required to be changed. */
		memcpy((void*)(destbuffer + odd), (const void*)srcbuffer, memcpylen);

		FLASH_Write(phyAddrAlign, (uint32_t *)destbuffer, FLASH_BYTES_ALIGNED/4);

		/* Update the address and size */
		start_addr = phyAddrAlign + FLASH_BYTES_ALIGNED;
		sizeLeft -= memcpylen;
		srcbuffer += memcpylen;
	}


	
	/* Now the startAddr is page size aligned. */
	while ( sizeLeft >= FLASH_BYTES_ALIGNED)
	{
		memcpy((void *)destbuffer, (const void*)srcbuffer, FLASH_BYTES_ALIGNED);
		FLASH_Write(start_addr, (uint32_t *)destbuffer, FLASH_BYTES_ALIGNED/4);
		start_addr += FLASH_BYTES_ALIGNED;
		sizeLeft -= FLASH_BYTES_ALIGNED;
		srcbuffer += FLASH_BYTES_ALIGNED;
	}

	if (sizeLeft > 0)
	{
		memcpy((void *)destbuffer, (const void*)srcbuffer, sizeLeft);
		memcpy((void *)(destbuffer + sizeLeft), (const void*)(start_addr + sizeLeft), FLASH_BYTES_ALIGNED - sizeLeft);
		FLASH_Write(start_addr, (uint32_t *)destbuffer, FLASH_BYTES_ALIGNED/4);
	}
	
	*off_set += in_buf_len;

	return 0;
	
#undef	FLASH_BYTES_ALIGNED	
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{
	uint32_t start_addr,i;
	hal_logic_partition_t  partition_info;
	hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
	hal_flash_info_get( pno, p_partition_info );

	if(poff == NULL || buf == NULL || *poff + buf_size > p_partition_info->partition_length)
	{
		return EIO;
	}
	
	start_addr = p_partition_info->partition_start_addr + *poff;

	for(i = 0;i < buf_size; i++)
	{
		*((uint8_t *)buf + i) = *((uint8_t *)(start_addr + i));
	}

	*poff += buf_size;

	return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
	uint32_t start_addr,end_addr,addr;
	hal_logic_partition_t  partition_info;
	hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
	hal_flash_info_get( pno, p_partition_info );

	if(size + off_set > p_partition_info->partition_length)
	{
		return EIO;
	}
	
	start_addr = (( p_partition_info->partition_start_addr + off_set ) / FLASH_BLOCK_SIZE ) * FLASH_BLOCK_SIZE;
	end_addr = (( p_partition_info->partition_start_addr + off_set + size) / FLASH_BLOCK_SIZE ) * FLASH_BLOCK_SIZE;


	for (addr = start_addr; addr <= end_addr; addr += FLASH_BLOCK_SIZE)
	{
		FLASH_Erase(addr);
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

