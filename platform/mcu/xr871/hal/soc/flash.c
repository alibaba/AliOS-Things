/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>

#include "prj_config.h"
#include "common/prj_conf_opt.h"

#include "aos/hal/flash.h"
#include "aos/hal/wdg.h"

#include "driver/chip/hal_chip.h"

#define XR871_FLASH	PRJCONF_IMG_FLASH
#define FLASH_READ 0
#define FLASH_WRITE 1
extern wdg_dev_t wdg;

#if 0

#define FIQMASK_REG_NAME_R "faultmask"
#define FIQMASK_REG_NAME_W "faultmask"

/*
 * Save the current interrupt enable state & disable FIQs
 */
static __always_inline unsigned long xr_fiq_save(void)
{
	unsigned long flags;

	__asm volatile(
		"mrs	%0, " FIQMASK_REG_NAME_R "\n"
		"cpsid	f"
		: "=r" (flags) : : "memory", "cc");
	return flags;
}

/*
 * restore saved FIQ state
 */
static __always_inline void xr_fiq_restore(unsigned long flags)
{
	__asm volatile(
		"msr	" FIQMASK_REG_NAME_W ", %0"
		:
		: "r" (flags)
		: "memory", "cc");
}

#define GLOBAL_INT_DECLARATION()   uint32_t fiq_tmp, irq_tmp
#define GLOBAL_INT_DISABLE()       do{\
										fiq_tmp = xr_fiq_save();\
										irq_tmp = xr_irq_save();\
									}while(0)


#define GLOBAL_INT_RESTORE()       do{                         \
										xr_fiq_restore(fiq_tmp);\
                                        xr_irq_restore(irq_tmp);\
                                   }while(0)
#else
#define GLOBAL_INT_DECLARATION()
#define GLOBAL_INT_DISABLE()
#define GLOBAL_INT_RESTORE()
#endif

#define SECTOR_SIZE 0x1000 /* 4 K/sector */

extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    GLOBAL_INT_DECLARATION();

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );

    if(size + off_set > p_partition_info->partition_length)
        return -1;

    start_addr = (p_partition_info->partition_start_addr + off_set) & (~0xFFF);
    end_addr = (p_partition_info->partition_start_addr + off_set + size - 1) & (~0xFFF);

    for(addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE)
    {
        hal_wdg_reload(&wdg);
        GLOBAL_INT_DISABLE();
        //flash_ctrl(CMD_FLASH_ERASE_SECTOR, &addr);
        //printf("flash_ctrl: addr=0x%08x\n", addr);
        flash_erase(XR871_FLASH, addr, SECTOR_SIZE);
        GLOBAL_INT_RESTORE();
    }
    hal_wdg_reload(&wdg);

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    GLOBAL_INT_DECLARATION();

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );

    if(off_set == NULL || in_buf == NULL || *off_set + in_buf_len > p_partition_info->partition_length)
        return -1;

    start_addr = p_partition_info->partition_start_addr + *off_set;

    hal_wdg_reload(&wdg);
    GLOBAL_INT_DISABLE();
    flash_rw(XR871_FLASH, start_addr, in_buf, in_buf_len, FLASH_WRITE);
	//printf("flash_write: addr=0x%08x,len=%d\n", start_addr, in_buf_len);
   // HAL_Flash_Write(XR871_FLASH, start_addr, in_buf, in_buf_len);
    GLOBAL_INT_RESTORE();
    hal_wdg_reload(&wdg);

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t start_addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    GLOBAL_INT_DECLARATION();

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );

    if(off_set == NULL || out_buf == NULL || *off_set + out_buf_len > p_partition_info->partition_length)
        return -1;

    start_addr = p_partition_info->partition_start_addr + *off_set;

    hal_wdg_reload(&wdg);
    GLOBAL_INT_DISABLE();
    //flash_read(out_buf, out_buf_len, start_addr);
	flash_rw(XR871_FLASH, start_addr, out_buf, out_buf_len, FLASH_READ);
    //printf("flash_read: addr=0x%08x,len=%d\n", start_addr, out_buf_len);
   // HAL_Flash_Read(XR871_FLASH, start_addr, out_buf, out_buf_len);
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
