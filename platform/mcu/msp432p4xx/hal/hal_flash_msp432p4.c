/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/hal/flash.h"
#include "k_api.h"
#include "Board.h"

#include <ti/drivers/NVS.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];
static NVS_Handle nvsHandle;
static NVS_Attrs nvs_attrs;
static int32_t nvs_inited = 0;

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

static int32_t nvs_handle_open(void)
{
    if (nvs_inited)
        return 0;

    NVS_init();

    nvsHandle = NVS_open(Board_NVSINTERNAL, NULL);
    if (nvsHandle == NULL) {
        return -1;
    }

    NVS_getAttrs(nvsHandle, &nvs_attrs);
    nvs_inited = 1;
    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();
    hal_flash_info_get(pno, p_partition_info);
    ret = nvs_handle_open();
    if (ret != 0) {
        return ret;
    }

    start_addr = p_partition_info->partition_start_addr + *poff - ((uint32_t)(nvs_attrs.regionBase));
    RHINO_CRITICAL_ENTER();
    ret = NVS_write(nvsHandle, start_addr, buf, buf_size, 0);
    RHINO_CRITICAL_EXIT();

    *poff += buf_size;
    return ret;

}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    int32_t ret = 0;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();
    hal_flash_info_get(pno, p_partition_info);
    ret = nvs_handle_open();
    if (ret != 0) {
        return ret;
    }

    start_addr = p_partition_info->partition_start_addr + *poff - ((uint32_t)(nvs_attrs.regionBase));;
    RHINO_CRITICAL_ENTER();
    ret = NVS_read(nvsHandle, start_addr, buf, buf_size);
    RHINO_CRITICAL_EXIT();

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    uint32_t addr, start_addr, end_addr, logic_addr;
    int32_t ret;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));

    CPSR_ALLOC();
    hal_flash_info_get(pno, p_partition_info);
    ret = nvs_handle_open();
    if (ret != 0) {
        return ret;
    }

    logic_addr = p_partition_info->partition_start_addr - (uint32_t)(nvs_attrs.regionBase);
    //printf("logic_addr 0x%x.\n", logic_addr);

    start_addr = ROUND_DOWN((logic_addr + off_set), nvs_attrs.sectorSize);
    end_addr = ROUND_DOWN((logic_addr + off_set + size - 1), nvs_attrs.sectorSize);

    for (addr = start_addr; addr <= end_addr; addr += nvs_attrs.sectorSize) {
        RHINO_CRITICAL_ENTER();
        ret = NVS_erase(nvsHandle, addr, size);
        RHINO_CRITICAL_EXIT();
        if (ret != 0) {
            return ret;
        }
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
