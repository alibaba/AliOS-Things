/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/aos.h"
#include "hal/soc/soc.h"

#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "flash_pub.h"
#include "ll.h"

extern wdg_dev_t wdg;

#define SECTOR_SIZE 0x1000 /* 4 K/sector */

extern const hal_logic_partition_t hal_partitions[];

struct flash_context
{
    aos_mutex_t lock;
    int         initialized;
};
static struct flash_context g_flash_context;

void hal_flash_init()
    {
    if (g_flash_context.initialized)
    {
        return;
    }
    if (0 == aos_mutex_new(&g_flash_context.lock))
    {
        g_flash_context.initialized = 1;
    }
}

static void hal_flash_lock()
{
    if (g_flash_context.initialized)
    {
        aos_mutex_lock(&g_flash_context.lock, AOS_WAIT_FOREVER);
    }
}

static void hal_flash_unlock()
{
    if (g_flash_context.initialized)
    {
        aos_mutex_unlock(&g_flash_context.lock);
    }
}

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
	uint32_t status;
    DD_HANDLE flash_hdl;

    //GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = (partition_info->partition_start_addr + off_set) & (~0xFFF);
    end_addr = (partition_info->partition_start_addr + off_set + size  - 1) & (~0xFFF);

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    for(addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE)
    {
        hal_wdg_reload(&wdg);
        //GLOBAL_INT_DISABLE();
        hal_flash_lock();
        ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&addr);
        //GLOBAL_INT_RESTORE();
        hal_flash_unlock();
    }
    hal_wdg_reload(&wdg);
	ddev_close(flash_hdl);
    
    return 0;
}
                        
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
	uint32_t status;
    DD_HANDLE flash_hdl;

    //GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *off_set;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    hal_wdg_reload(&wdg);
    //GLOBAL_INT_DISABLE();
    hal_flash_lock();
    ddev_write(flash_hdl, in_buf, in_buf_len, start_addr);
    //GLOBAL_INT_RESTORE();
    hal_flash_unlock();
    hal_wdg_reload(&wdg);
	ddev_close(flash_hdl);

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
	uint32_t status;
    DD_HANDLE flash_hdl;

    //GLOBAL_INT_DECLARATION();

    partition_info = hal_flash_get_info( in_partition );

    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > partition_info->partition_length)
        return -1;

    start_addr = partition_info->partition_start_addr + *off_set;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    hal_wdg_reload(&wdg);
    //GLOBAL_INT_DISABLE();
    hal_flash_lock();
    ddev_read(flash_hdl, out_buf, out_buf_len, start_addr);
    //GLOBAL_INT_RESTORE();
    hal_flash_unlock();
    hal_wdg_reload(&wdg);
	ddev_close(flash_hdl);

    *off_set += out_buf_len;

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
	DD_HANDLE flash_hdl;
    UINT32 status;
	uint32_t param = FLASH_UNPROTECT_LAST_BLOCK;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&param);

    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
	DD_HANDLE flash_hdl;
    UINT32 status;
	uint32_t param = FLASH_PROTECT_NONE;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&param);

    return 0;
}
