/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "mico_rtos.h"

typedef unsigned char  		  UINT8;          /* Unsigned  8 bit quantity        */
typedef signed   char  		  INT8;           /* Signed    8 bit quantity        */
typedef unsigned short 		  UINT16;         /* Unsigned 16 bit quantity        */
typedef signed   short 		  INT16;          /* Signed   16 bit quantity        */
typedef uint32_t   		      UINT32;         /* Unsigned 32 bit quantity        */
typedef int32_t   		      INT32;          /* Signed   32 bit quantity        */

#include "flash_pub.h"

extern wdg_dev_t wdg;

#define GLOBAL_INT_DECLARATION()   uint32_t fiq_tmp, irq_tmp
#define GLOBAL_INT_DISABLE()       do{\
										fiq_tmp = portDISABLE_FIQ();\
										irq_tmp = portDISABLE_IRQ();\
									}while(0)


#define GLOBAL_INT_RESTORE()       do{                         \
                                        if(!fiq_tmp)           \
                                        {                      \
                                            portENABLE_FIQ();    \
                                        }                      \
                                        if(!irq_tmp)           \
                                        {                      \
                                            portENABLE_IRQ();    \
                                        }                      \
                                   }while(0)

#define SECTOR_SIZE 0x1000 /* 4 K/sector */

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];

    return logic_partition;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    hal_logic_partition_t *partition_info;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
        if (in_partition == CONFIG_AOS_KV_PTN) {
            if (off_set >= CONFIG_AOS_KV_PTN_SIZE) {
                in_partition = CONFIG_AOS_KV_SECOND_PTN;
                off_set -= CONFIG_AOS_KV_PTN_SIZE;
            }
        }
#endif


    GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = (partition_info->partition_start_addr + off_set) & (~0xFFF);
    end_addr = (partition_info->partition_start_addr + off_set + size  - 1) & (~0xFFF);

    for(addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE)
    {
        hal_wdg_reload(&wdg);
        GLOBAL_INT_DISABLE();
        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &addr);
        GLOBAL_INT_RESTORE();
    }
    hal_wdg_reload(&wdg);
    
    return 0;
}
                        
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
        if (in_partition == CONFIG_AOS_KV_PTN) {
            if ((*off_set) >= CONFIG_AOS_KV_PTN_SIZE) {
                in_partition = CONFIG_AOS_KV_SECOND_PTN;
                *off_set = (*off_set) - CONFIG_AOS_KV_PTN_SIZE;
            }
        }
#endif

    GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *off_set;

    hal_wdg_reload(&wdg);
    GLOBAL_INT_DISABLE();
    flash_write(in_buf, in_buf_len, start_addr);
    GLOBAL_INT_RESTORE();
    hal_wdg_reload(&wdg);

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;

#ifdef CONFIG_AOS_KV_MULTIPTN_MODE
    if (in_partition == CONFIG_AOS_KV_PTN) {
        if ((*off_set) >=  CONFIG_AOS_KV_PTN_SIZE) {
            in_partition = CONFIG_AOS_KV_SECOND_PTN;
            *off_set = (*off_set) - CONFIG_AOS_KV_PTN_SIZE;
        }
    }
#endif

    GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *off_set;

    hal_wdg_reload(&wdg);
    GLOBAL_INT_DISABLE();
    flash_read(out_buf, out_buf_len, start_addr);
    GLOBAL_INT_RESTORE();
    hal_wdg_reload(&wdg);

    *off_set += out_buf_len;

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
