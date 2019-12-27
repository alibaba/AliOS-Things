#include <string.h>
#include "chip_code.h"

//第4个成员name，须与mk或ucube.py中设置的 HOST_MCU_FAMILY 值保持一致
chip_code_st g_chip_codes[] = {
    {BEKEN_CHIP_VENDOR, bk7231, "mcu_bk7231"}, //0x00010001
    {BEKEN_CHIP_VENDOR, bk7231u, "mcu_bk7231u"}, //0x00010002
    {C_SKY_CHIP_VENDOR, csky, "mcu_csky"}, //0x00020003
    {CYPRESS_CHIP_VENDOR, cy8c4147, "mcu_cy8c4147"}, //0x00030004
    {CYPRESS_CHIP_VENDOR, cy8c6347, "mcu_cy8c6347"}, //0x00030005
    {DAHUA_CHIP_VENDOR, dahua, "mcu_dahua"}, //0x00040006
    {ESP_CHIP_VENDOR, es8p508x, "mcu_es8p508x"}, //0x00050007
    {ESP_CHIP_VENDOR, esp32, "mcu_esp32"}, //0x00050008
    {ESP_CHIP_VENDOR, esp8266, "mcu_esp8266"}, //0x00050009
    {GD_CHIP_VENDOR, gd32f4xx, "mcu_gd32f4xx"}, //0x0006000a
    {LINUX_VENDOR, linuxhost, "mcu_linuximpl"}, //0x0007000b
    {NXP_VENDOR, lpc54102, "mcu_lpc54102impl"}, //0x0008000c
    {NXP_VENDOR, mkl27z644, "mcu_mkl27z644impl"}, //0x0008000d
    {MICO_CHIP_VENDOR, moc108, "mcu_moc108"}, //0x0009000e
    {MICO_CHIP_VENDOR, mx1290, "mcu_mx1290"}, //0x0009000f  为了满足和庆科约定，mk3080的庆科单独埋点上报这个值，否则是rtl8710bn，两者实际是一样的
    {MICO_CHIP_VENDOR, mx1101, "mcu_mx1101"}, //0x00090020
    {NORDIC_CHIP_VENDOR, nrf52xxx, "mcu_nrf52xxx"}, //0x000a0010
    {RENASAS_CHIP_VENDOR, r5f100lea, "mcu_r5f100lea"}, //0x000b0011
    {RENASAS_CHIP_VENDOR, r5f565ne, "mcu_r5f565ne"}, //0x000b0012
    {RENASAS_CHIP_VENDOR, r7f0c004, "mcu_r7f0c004"}, //0x000b0013
    {RDA_CHIP_VENDOR, rda5981x, "mcu_rda5981x"}, //0x000c0014
    {RDA_CHIP_VENDOR, rda8955, "mcu_rda8955"}, //0x000c0015
    {REALTEK_CHIP_VENDOR, rtl8710bn, "mcu_rtl8710bn"}, //0x000d0016
    {STM_CHIP_VENDOR, stm32f4xx, "mcu_stm32f4xx"}, //0x000e0017
    {STM_CHIP_VENDOR, stm32f4xx_cube, "mcu_stm32f4xx_cube"}, //0x000e0018
    {STM_CHIP_VENDOR, stm32f7xx, "mcu_stm32f7xx"}, //0x000e0019
    {STM_CHIP_VENDOR, stm32l0xx, "mcu_stm32l0xx"}, //0x000e001a
    {STM_CHIP_VENDOR, stm32l475, "mcu_stm32l475"}, //0x000e001b
    {STM_CHIP_VENDOR, stm32l4xx, "mcu_stm32l4xx"}, //0x000e001c
    {STM_CHIP_VENDOR, stm32l4xx_cube, "mcu_stm32l4xx_cube"}, //0x000e001d
    {XRADIO_CHIP_VENDOR, xr871, "mcu_xr871"}, //0x000f001e
    {MTK_CHIP_VENDOR, mtk6261m, "mcu_mtk6261m"}, //0x0010001f
};

chip_code_st *get_chip_code( char *name)
{
    UINT32 length = sizeof(g_chip_codes) / sizeof(chip_code_st);
    UINT32 i;
    for ( i = 0; i < length; i++ ) {
        if ( strcmp( g_chip_codes[i].mcu_family_name, name) == 0 ) {
            break;
        }
    }

    if ( i >= length ) {
        return NULL;
    }

    return &g_chip_codes[i];
}
