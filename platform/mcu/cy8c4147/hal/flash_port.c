#include "hal/soc/soc.h"
#include <aos/aos.h>
#include <stdio.h>
#include <string.h>
#include <CyFlash.h>

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

#define FLASH_PAGE_SIZE (CY_FLASH_SIZEOF_ROW)

extern const hal_logic_partition_t hal_partitions[];

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
    int remaining = size;
    uint8_t * src_addr = (uint8_t *) data;
    uint32 status;

    uint32_t * page_cache = NULL;
    page_cache = (uint32_t *)aos_malloc(FLASH_PAGE_SIZE);
    if (page_cache == NULL)
        return -1;
    memset(page_cache, 0, FLASH_PAGE_SIZE);

    do {
        uint32_t fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        int fl_offset = dst_addr - fl_addr;
        int len = MIN(FLASH_PAGE_SIZE - fl_offset, size);

        /* Load from the flash into the cache */
        memcpy(page_cache, (void *) fl_addr, FLASH_PAGE_SIZE);  
        /* Update the cache from the source */
        memcpy((uint8_t *)page_cache + fl_offset, src_addr, len);

        status = CySysFlashWriteRow(fl_addr/FLASH_PAGE_SIZE, (uint8 *)page_cache);
        if(status != CY_SYS_FLASH_SUCCESS)
        {
            printf("Error writing %u bytes at 0x%08x\n", FLASH_PAGE_SIZE, fl_addr);
        }
        else
        {
            dst_addr += len;
            src_addr += len;
            remaining -= len;
        }
    }
    while ((status == CY_SYS_FLASH_SUCCESS) && (remaining > 0));

    aos_free(page_cache);
    
    return 0;
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
int FLASH_read_at(uint32_t address, uint8_t *pData, uint32_t len_bytes)
{
    uint32 i;
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

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;
		
    partition_info = hal_flash_get_info( real_pno );
    start_addr = partition_info->partition_start_addr + *poff;
    
    if (0 != FLASH_update(start_addr, buf, buf_size)) {
        printf("FLASH_update failed!\n");
    }
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if ((*poff) >=  CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            *poff = (*poff) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t start_addr;
    uint32 status;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;
    uint32_t * page_cache = NULL;

    page_cache = (uint32_t *)aos_malloc(FLASH_PAGE_SIZE);
    if (page_cache == NULL)
      return -1;
    memset(page_cache, 0xff, FLASH_PAGE_SIZE);

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (pno == CONFIG_AOS_KV_PTN) {
        if (off_set >= CONFIG_AOS_KV_PTN_SIZE) {
            pno = (hal_partition_t)CONFIG_AOS_KV_SECOND_PTN;
            off_set -= CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );
    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);

    for(uint32_t i=0; i<size/FLASH_PAGE_SIZE; i++)
    {
        status = CySysFlashWriteRow(start_addr/FLASH_PAGE_SIZE, (uint8 *)page_cache);
  
        if(status != CY_SYS_FLASH_SUCCESS)
        {
            printf("Flash erase failed\n");
        }
        start_addr += FLASH_PAGE_SIZE;
    }
    aos_free(page_cache);

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    (void)partition;
    (void)off_set;
    (void)size;
    
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    (void)partition;
    (void)off_set;
    (void)size;
    
    return 0;
}
