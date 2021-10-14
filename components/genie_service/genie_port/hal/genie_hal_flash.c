/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include "soc.h"
#include "drv/spiflash.h"
//#include "dut/hal/common.h"
//#include "dut/hal/ble.h"
#include "aos/hal/flash.h"
#include "aos/kernel.h"
#include "common/log.h"

#define FLASH_ALIGN_MASK ~(sizeof(uint32_t) - 1)
#define FLASH_ALIGN sizeof(uint32_t)

//#define MAC_PARAMS_OFFSET (0)
//#define MAC_PARAMS_SIZE (0x10)

//#define RSV_PARAMS_OFFSET (MAC_PARAMS_OFFSET + MAC_PARAMS_SIZE)
//#define RSV_PARAMS_SIZE (0x10)

//#define FREQ_PARAMS_OFFSET (RSV_PARAMS_OFFSET + RSV_PARAMS_SIZE)
//#define FREQ_PARAMS_SIZE (0x8)

//#define TRIPLE_OFFSET (FREQ_PARAMS_OFFSET + FREQ_PARAMS_SIZE)
#define TRIPLE_OFFSET (0)
#define TRIPLE_SIZE (32)

#define GROUP_ADDR_OFFSET (TRIPLE_OFFSET + TRIPLE_SIZE)
#define GROUP_ADDR_SIZE (18) //CONFIG_BT_MESH_MODEL_GROUP_COUNT * 2 + 2

#define SN_PARAMS_OFFSET (GROUP_ADDR_OFFSET + GROUP_ADDR_SIZE)
#define SN_PARAMS_SIZE (32)

//#define OTP_TOTAL_DATA_SIZE (MAC_PARAMS_SIZE + RSV_PARAMS_SIZE + FREQ_PARAMS_SIZE + TRIPLE_SIZE + GROUP_ADDR_SIZE + SN_PARAMS_SIZE)
#define OTP_TOTAL_DATA_SIZE (TRIPLE_SIZE + GROUP_ADDR_SIZE + SN_PARAMS_SIZE)

int32_t hal_flash_read_otp(uint32_t off_set, void *out_buf, uint32_t out_buf_len)
{
    //return dut_hal_factorydata_read(off_set, out_buf, out_buf_len);
}

int32_t hal_flash_write_otp(uint32_t off_set, const void *in_buf, uint32_t in_buf_len)
{
    int32_t retval = 0;

    if (off_set + in_buf_len > OTP_TOTAL_DATA_SIZE)
    {
        printf("param err\n");
        return -1;
    }

    //retval = dut_hal_factorydata_store(off_set, (uint8_t *)in_buf, in_buf_len);

    return retval;
}

int32_t hal_flash_read_xtalcap_params(void *out_buf, uint32_t out_buf_len)
{
    uint32_t xtal_cap = 0;
    int ret = -1;
    if (out_buf_len != sizeof(uint32_t)) {
        return -1;
    }

    //ret = dut_hal_xtalcap_get(&xtal_cap);

    memcpy(out_buf, &xtal_cap, sizeof(uint32_t));

    return ret;
}

int32_t hal_flash_write_xtalcap_params(const void *in_buf, uint32_t in_buf_len)
{
    if (in_buf_len != sizeof(uint32_t)) {
        return -1;
    }

    //return dut_hal_xtalcap_store(*((uint32_t *)in_buf));
}

int32_t hal_flash_read_sn_params(void *out_buf, uint32_t out_buf_len)
{
    return hal_flash_read_otp(SN_PARAMS_OFFSET, out_buf, out_buf_len);
}

int32_t hal_flash_write_sn_params(const void *in_buf, uint32_t in_buf_len)
{
    return hal_flash_write_otp(SN_PARAMS_OFFSET, in_buf, in_buf_len);
}

int32_t hal_flash_read_mac_params(void *out_buf, uint32_t out_buf_len)
{
    if (!out_buf || out_buf_len < 6)
        return -1;

    //return dut_hal_mac_get((uint8_t *)out_buf);
}

int32_t hal_flash_write_mac_params(const void *in_buf, uint32_t in_buf_len)
{
    if (!in_buf || in_buf_len != 6)
        return -1;

    //return dut_hal_mac_store((uint8_t *)in_buf);
}

int32_t hal_flash_read_triples(void *out_buf, uint32_t out_buf_len)
{
    return hal_flash_read_otp(TRIPLE_OFFSET, out_buf, out_buf_len);
}

int32_t hal_flash_write_triples(const void *in_buf, uint32_t in_buf_len)
{
    return hal_flash_write_otp(TRIPLE_OFFSET, in_buf, in_buf_len);
}

int32_t hal_flash_read_group_addr(void *out_buf, uint32_t out_buf_len)
{
    return hal_flash_read_otp(GROUP_ADDR_OFFSET, out_buf, out_buf_len);
}

int32_t hal_flash_write_group_addr(const void *in_buf, uint32_t in_buf_len)
{
    return hal_flash_write_otp(GROUP_ADDR_OFFSET, in_buf, in_buf_len);
}

int32_t hal_flash_add2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr)
{ //partition_start_addr
    hal_logic_partition_t partition_info;
    int ret;
    for (int i = 0; i < HAL_PARTITION_MAX; i++)
    {
        ret = hal_flash_info_get(i, &partition_info);
        if (ret)
            continue;

        if ((addr >= partition_info.partition_start_addr) && (addr < (partition_info.partition_start_addr + partition_info.partition_length)))
        {
            *in_partition = i;
            *off_set = addr - partition_info.partition_start_addr;
            return 0;
        }
    }
    *in_partition = -1;
    *off_set = 0;
    return -1;
}

void hal_flash_write_test()
{
    int ret = 0;
    uint32_t offset = 0;
    uint8_t data[32] = {0};
    uint8_t data_read[32] = {0};
    int i = 0;
    for (i = 0; i < sizeof(data); i++)
    {
        data[i] = i;
    }

    hal_flash_erase(HAL_PARTITION_PARAMETER_2, 0, 4096);

    offset = 0;

    //printf("write test 32 by 32 %d, offset %d size %d, data %p\n", HAL_PARTITION_PARAMETER_2, offset, 4096, data);
    for (i = 0; i < 4096 / 32; i++)
    {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_2, &offset, (uint8_t *)(data), 32);
        if (ret)
        {
            printf("write 1 fail %d\n", ret);
            break;
        }
    }

    offset = 0;
    memset(data_read, 0, 32);
    for (i = 0; i < 4096 / 32; i++)
    {
        memset(data_read, 0, 32);
        ret = hal_flash_read(HAL_PARTITION_PARAMETER_2, &offset, (uint8_t *)(data_read), 32);
        if (ret)
        {
            printf("read 1 fail %d\n", ret);
            break;
        }
        if (memcmp(data, data_read, 32))
        {
            printf("write test fail, data missmatch\n");
            break;
        }
    }

    hal_flash_erase(HAL_PARTITION_PARAMETER_2, 0, 4096);

    offset = 0;

    //printf("write test 1 by 1 %d, offset %d size %d, data %p\n", HAL_PARTITION_PARAMETER_2, offset, 4096, data);
    for (i = 0; i < 4096; i++)
    {
        ret = hal_flash_write(HAL_PARTITION_PARAMETER_2, &offset, (uint8_t *)(data + i % 32), 1);
        if (ret)
        {
            printf("write 2 fail %d\n", ret);
            break;
        }
    }

    offset = 0;
    memset(data_read, 0, 32);
    for (i = 0; i < 4096; i++)
    {
        ret = hal_flash_read(HAL_PARTITION_PARAMETER_2, &offset, (uint8_t *)(data_read + i % 32), 1);
        if (ret)
        {
            printf("read 2 fail %d\n", ret);
            break;
        }
        if ((i + 1) % 32 == 0)
        {
            if (memcmp(data, data_read, 32))
            {
                printf("write 2 1 by 1 test fail, data missmatch\n");
                break;
            }
            memset(data_read, 0, 32);
        }
    }

    hal_flash_erase(HAL_PARTITION_PARAMETER_2, 0, 4096);
}
