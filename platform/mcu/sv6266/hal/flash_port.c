#include "aos/hal/flash.h"
#include "sys/flash.h"
#include "osal.h"
#include "wdt/drv_wdt.h"
#include "errno.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))


static int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    uint32_t remaining = size;
    uint32_t fl_addr; 
    uint32_t fl_offset;
    uint32_t len;
    uint8_t* src_addr = (uint8_t *) data;

    OS_DeclareCritical();

    do
    {
        fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        fl_offset = dst_addr - fl_addr;
        len = MIN(FLASH_PAGE_SIZE - fl_offset, remaining);
        
#if defined(CONFIG_ENABLE_WDT)
        drv_wdt_kick(SYS_WDT);
#endif
//        if(fl_offset)
//        {
            OS_EnterCritical();
            flash_page_program(dst_addr, len, src_addr);
            OS_ExitCritical();	
//		}
//		else
//		{
//            OS_EnterCritical();
//            flash_page_program(fl_addr, len, src_addr);
//            OS_ExitCritical();
//		}
		
        {
			dst_addr += len;
			src_addr += len;
			remaining -= len;
        }
    } while(remaining > 0);

    return 0;
}

int FLASH_read_at(uint32_t address, uint8_t* pData, uint32_t len_bytes)
{
    uint8_t *src = (uint8_t*)(address+0x30000000);

    while(len_bytes--)
    {
        *pData++ = *src++;
    }

    return 0;
}

int FLASH_unlock_erase(uint32_t address, uint32_t len_bytes)
{
    uint32_t i = 0;
    uint32_t erase_count = (len_bytes + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
    OS_DeclareCritical();

    for (i = 0; i < erase_count; i++)
    {
        OS_EnterCritical();
#if defined(CONFIG_ENABLE_WDT)
        drv_wdt_kick(SYS_WDT);
#endif
        flash_sector_erase(address + (i * FLASH_SECTOR_SIZE));
        OS_ExitCritical();
    }
    return 0;	
}

extern const hal_logic_partition_t hal_partitions[];
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
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );
    start_addr = partition_info->partition_start_addr + *poff;
    if (buf_size == 0) {
        return 0;
    }
    if ((poff == NULL) || (buf == NULL) || ((*poff + buf_size) > partition_info->partition_length)) {
        // overwrite.
        return EIO;
    }
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
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );

    if (buf_size == 0) {
        return 0;
    }
    if ((poff == NULL) || (buf == NULL) || ((*poff + buf_size) > partition_info->partition_length)) {
        return EIO;
    }
    start_addr = partition_info->partition_start_addr + *poff;
    FLASH_read_at(start_addr, buf, buf_size);
    *poff += buf_size;

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );
    if ((size + off_set) > partition_info->partition_length) {
        return EIO;
    }

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), FLASH_PAGE_SIZE);

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



