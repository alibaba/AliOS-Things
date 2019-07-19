
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <k_api.h>
#include "aos/hal/flash.h"
#include "errno.h"
#include "port_mcu.h"
#include "em_msc.h"
#include "efm32g222f128.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

uint8_t mempool[FLASH_PAGE_SIZE];

hal_logic_partition_t hal_logic_partition[HAL_PARTITION_MAX] = 
{
    [HAL_PARTITION_BOOTLOADER]  = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_APPLICATION] = {HAL_FLASH_EMBEDDED   ,"Application"  ,0x410             ,0xFBF0  ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_EN},
    [HAL_PARTITION_ATE]         = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_OTA_TEMP]    = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_RF_FIRMWARE] = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_PARAMETER_1] = {HAL_FLASH_EMBEDDED   ,"Parameter1"   ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_PARAMETER_2] = {HAL_FLASH_EMBEDDED   ,"Parameter2"   ,1024*120          ,1024*8  ,PAR_OPT_WRITE_EN|PAR_OPT_READ_EN},
    [HAL_PARTITION_PARAMETER_3] = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_PARAMETER_4] = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
    [HAL_PARTITION_BT_FIRMWARE] = {HAL_FLASH_NONE       ,NULL           ,0                 ,0       ,PAR_OPT_WRITE_DIS|PAR_OPT_READ_DIS},
};

void EFM32_EFM32_FLASH_Init(void)
{
    MSC_Init();
}

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_logic_partition[ pno ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    int32_t             status = 0;
    int                 erase_size = size;
    uint32_t            start_address;
    uint32_t            page_num = 0;
    uint32_t            i;
    hal_logic_partition_t info;
    hal_logic_partition_t *logic = &info;

    if (hal_flash_info_get(pno, logic) != 0) {
        return 5;
    }

    if((0 == logic->partition_start_addr) || (0 == logic->partition_length)) {
        return 5;
    }

    if(logic->partition_options & PAR_OPT_WRITE_DIS) {
        return 5;
    }

    if(HAL_FLASH_EMBEDDED != logic->partition_owner) {
        return 5;
    }

    if((off_set + size) > logic->partition_length) {
        return 5;
    }

    start_address = ROUND_DOWN((logic->partition_start_addr + off_set), FLASH_PAGE_SIZE);

    erase_size += (logic->partition_start_addr + off_set - start_address);
    page_num    = (erase_size + FLASH_PAGE_SIZE - 1)/FLASH_PAGE_SIZE;

    __disable_irq();
    for (i = 0; i < page_num; i++) {
        uint32_t erase_address;

        erase_address = start_address + i * FLASH_PAGE_SIZE;

        status = MSC_ErasePage((uint32_t *)erase_address);
    }
    __enable_irq();

    if (status == 0) {
        return 0;
    }
    else {
        return 5;
    }
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    int32_t     status = 0;
    uint32_t    address = 0;
    uint32_t    fl_addr;
    uint32_t    fl_offset;
    uint32_t    i;
    uint32_t    remain_size = in_buf_len;
    uint32_t    write_size;
    uint8_t    *p_inbuf = (uint8_t *)in_buf;

    hal_logic_partition_t info;
    hal_logic_partition_t *logic = &info;

    if (hal_flash_info_get(pno, logic) != 0) {
        return 5;
    }

    if((0 == logic->partition_start_addr) || (0 == logic->partition_length)) {
        return 5;
    }

    if(logic->partition_options & PAR_OPT_WRITE_DIS) {
        return 5;
    }

    if(HAL_FLASH_EMBEDDED != logic->partition_owner) {
        return 5;
    }

    if((*off_set + in_buf_len) > logic->partition_length) {
        return 5;
    }

    address = logic->partition_start_addr + *off_set;
    fl_addr     = ROUND_DOWN(address, FLASH_PAGE_SIZE);
    fl_offset   = address - fl_addr;

    while (remain_size > 0) {
        memcpy(mempool, (void *)fl_addr, FLASH_PAGE_SIZE);

        write_size = MIN(remain_size, FLASH_PAGE_SIZE - fl_offset);

        memcpy(mempool + fl_offset, p_inbuf, write_size);

        __disable_irq();
        status = MSC_WriteWord((uint32_t *)fl_addr, mempool, FLASH_PAGE_SIZE);
        __enable_irq();

        p_inbuf     += write_size;
        remain_size -= write_size;
        fl_offset    = 0;
        fl_addr     += FLASH_PAGE_SIZE;
    }

    *off_set += in_buf_len;

    if (status == 0) {
        return 0;
    }
    else {
        return 5;
    }
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *off_set, void *out_buf, uint32_t in_buf_len)
{
    uint32_t address = 0;

    hal_logic_partition_t info;
    hal_logic_partition_t *logic = &info;

    if (hal_flash_info_get(pno, logic) != 0) {
        return 5;
    }

    if((0 == logic->partition_start_addr) || (0 == logic->partition_length)) {
        return 5;
    }

    if(logic->partition_options & PAR_OPT_READ_DIS) {
        return 5;
    }

    if(HAL_FLASH_EMBEDDED != logic->partition_owner) {
        return 5;
    }

    address = logic->partition_start_addr + *off_set;

    memcpy(out_buf, (void *)address, in_buf_len);

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t pno, uint32_t off_set, uint32_t size)
{
    return 0;
}



