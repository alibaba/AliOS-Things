#include "2ndboot.h"
#include "ameba_soc.h"
#include "build_info.h"
#include "strproc.h"
#include "system_8195a.h"

void ota_2ndboot_os_entry(void)
{
    uint8_t  buf[0x30];
    uint32_t bd_addr = 0;
    uint32_t bd_len = 0;
    uint32_t start_addr = 0;
    uint32_t os_entry_section_addr[2] = {0, 0};
    hal_logic_partition_t *part_info = flash_get_info(HAL_PARTITION_APPLICATION);
    void (*RamStartFun) (void);
    if(NULL == part_info) {
	return;
    }
    start_addr = part_info->partition_start_addr;
    flash_read_data(start_addr, buf, 16);
    bd_addr = start_addr + *(uint32_t *)&buf[8] + 0x20;
    flash_read_data(bd_addr, buf, 0x30);

    os_entry_section_addr[0] = *(uint32_t *)(&buf[0x20]);
    os_entry_section_addr[1] = *(uint32_t *)(&buf[0x24]);
    bd_len = *(uint32_t *)(&buf[0x8]);
    printf("os entry section addr 0x%x, 0x%x\n", os_entry_section_addr[0], os_entry_section_addr[1]);
    /* copy OS data section to BD_RAM 0x10005000 */
    flash_read_data(bd_addr + 0x20, 0x10005000, bd_len);

    /* jump to OS entry */
    RamStartFun = os_entry_section_addr[0];
    RamStartFun();
    return;
}

void ota_2ndboot_image_entry(void)
{
    /* clear BSS */
    uint32_t BssLen = (__bss_end__ - __bss_start__);
    _memset((void *) __bss_start__, 0, BssLen);
    printf("2ndboot image start \n");
    ota_2ndboot_main();
    /* normal start */
    printf("os image start\r\n");
    ota_2ndboot_os_entry();
}

void ota_2ndboot_wakeup(void)
{
    uint8_t  buf[0x30];
    uint32_t bd_addr = 0;
    uint32_t bd_len = 0;
    uint32_t start_addr;
    uint32_t os_entry_section_addr[2] = {0, 0};
    hal_logic_partition_t *part_info = flash_get_info(HAL_PARTITION_APPLICATION);
    void (*RamStartFun) (void);
    if(NULL == part_info) {
	return;
    }
    start_addr = part_info->partition_start_addr;
    flash_read_data(start_addr, buf, 16);
    bd_addr = start_addr + *(uint32_t *)&buf[8] + 0x20;
    flash_read_data(bd_addr, buf, 0x30);
    os_entry_section_addr[0] = *(uint32_t *)(&buf[0x20]);
    os_entry_section_addr[1] = *(uint32_t *)(&buf[0x24]);
    bd_len = *(uint32_t *)(&buf[0x8]);
    /* Jump to wakeup function */
    RamStartFun = os_entry_section_addr[1];
    RamStartFun();
    return;
}

IMAGE2_VALID_PATTEN_SECTION
const u8 RAM_IMG2_VALID_PATTEN[20] = {
    'R', 'T', 'K', 'W', 'i', 'n', 0x0, 0xff,
    (FW_VERSION&0xff), ((FW_VERSION >> 8)&0xff),
    (FW_SUBVERSION&0xff), ((FW_SUBVERSION >> 8)&0xff),
    (FW_CHIP_ID&0xff), ((FW_CHIP_ID >> 8)&0xff),
    (FW_CHIP_VER),
    (FW_BUS_TYPE),
    (FW_INFO_RSV1),
    (FW_INFO_RSV2),
    (FW_INFO_RSV3),
    (FW_INFO_RSV4)
};

IMAGE2_ENTRY_SECTION RAM_START_FUNCTION gImage2EntryFun0 = { ota_2ndboot_image_entry, ota_2ndboot_wakeup };
