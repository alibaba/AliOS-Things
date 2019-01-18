#include <string.h>
#include "chip_code.h"

//第4个成员name，须与mk或ucube.py中设置的 HOST_MCU_FAMILY 值保持一致
chip_code_st g_chip_codes[] = {
    {BEKEN_CHIP_VENDOR, bk7231, "bk7231"}, //0x00010001
    {BEKEN_CHIP_VENDOR, bk7231u, "bk7231u"}, //0x00010002
    {C_SKY_CHIP_VENDOR, csky, "csky"}, //0x00020003
    {CYPRESS_CHIP_VENDOR, cy8c4147, "cy8c4147"}, //0x00030004
    {CYPRESS_CHIP_VENDOR, cy8c6347, "cy8c6347"}, //0x00030005
    {DAHUA_CHIP_VENDOR, dahua, "dahua"}, //0x00040006
    {ESP_CHIP_VENDOR, es8p508x, "es8p508x"}, //0x00050007
    {ESP_CHIP_VENDOR, esp32, "esp32"}, //0x00050008
    {ESP_CHIP_VENDOR, esp8266, "esp8266"}, //0x00050009
    {GD_CHIP_VENDOR, gd32f4xx, "gd32f4xx"}, //0x0006000a
    {LINUX_VENDOR, linuxhost, "linux"}, //0x0007000b
    {NXP_VENDOR, lpc54102, "lpc54102"}, //0x0008000c
    {NXP_VENDOR, mkl27z644, "mkl27z644"}, //0x0008000d
    {MICO_CHIP_VENDOR, moc108, "moc108"}, //0x0009000e
    {MICO_CHIP_VENDOR, mx1290, "mx1290"}, //0x0009000f
    {MICO_CHIP_VENDOR, mx1101, "mx1101"}, //0x00090020
    {NORDIC_CHIP_VENDOR, nrf52xxx, "nrf52xxx"}, //0x000a0010
    {RENASAS_CHIP_VENDOR, r5f100lea, "r5f100lea"}, //0x000b0011
    {RENASAS_CHIP_VENDOR, r5f565ne, "r5f565ne"}, //0x000b0012
    {RENASAS_CHIP_VENDOR, r7f0c004, "r7f0c004"}, //0x000b0013
    {RDA_CHIP_VENDOR, rda5981x, "rda5981x"}, //0x000c0014
    {RDA_CHIP_VENDOR, rda8955, "rda8955"}, //0x000c0015
    {REALTEK_CHIP_VENDOR, rtl8710bn, "rtl8710bn"}, //0x000d0016
    {STM_CHIP_VENDOR, stm32f4xx, "stm32f4xx"}, //0x000e0017
    {STM_CHIP_VENDOR, stm32f4xx_cube, "stm32f4xx_cube"}, //0x000e0018
    {STM_CHIP_VENDOR, stm32f7xx, "stm32f7xx"}, //0x000e0019
    {STM_CHIP_VENDOR, stm32l0xx, "stm32l0xx"}, //0x000e001a
    {STM_CHIP_VENDOR, stm32l475, "stm32l475"}, //0x000e001b
    {STM_CHIP_VENDOR, stm32l4xx, "stm32l4xx"}, //0x000e001c
    {STM_CHIP_VENDOR, stm32l4xx_cube, "stm32l4xx_cube"}, //0x000e001d
    {XRADIO_CHIP_VENDOR, xr871, "xr871"}, //0x000f001e
    {MTK_CHIP_VENDOR, mtk6261m, "mtk6261m"}, //0x0010001f
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
