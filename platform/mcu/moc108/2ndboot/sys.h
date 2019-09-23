
#ifndef __2NDBOOT_SYS_H__
#define __2NDBOOT_SYS_H__
#include "bootloader.h"

/* ICU MACROS */
#define REG_ICU_BASE_ADDR                   (0x00802000UL)

#define REG_ICU_PERI_CLK_PWD_ADDR           (REG_ICU_BASE_ADDR + 0x2 * 4)
#define REG_ICU_PERI_CLK_PWD_MASK           0x000BFFFFUL
#define REG_ICU_PERI_CLK_PWD                (*((volatile unsigned long *) REG_ICU_PERI_CLK_PWD_ADDR))

#define ICU_PERI_CLK_PWD_SET(x)             do {REG_ICU_PERI_CLK_PWD |=  (x);} while(0)
#define ICU_PERI_CLK_PWD_CLEAR(x)           do {REG_ICU_PERI_CLK_PWD &= ~(x);} while(0)

#define ICU_PERI_CLK_PWD_ARM_WDT_POSI       8
#define ICU_PERI_CLK_PWD_ARM_WDT_MASK       (0x01UL << ICU_PERI_CLK_PWD_ARM_WDT_POSI)

/* WDT MACROS */
#define REG_WDT_BASE_ADDR                   (0x00802900UL)
#define REG_WDT_CONFIG_ADDR                 (REG_WDT_BASE_ADDR + 0*4)
#define REG_WDT_CONFIG_MASK                 0x00FFFFFFUL
#define REG_WDT_CONFIG                      (*((volatile unsigned long *) REG_WDT_CONFIG_ADDR))

#define WDKEY_ENABLE1                       0x005A
#define WDKEY_ENABLE2                       0x00A5

#define WDT_CONFIG_PERIOD_POSI              0
#define WDT_CONFIG_PERIOD_MASK              (0x0000FFFFUL << WDT_CONFIG_PERIOD_POSI)

#define WDT_CONFIG_WDKEY_POSI               16
#define WDT_CONFIG_WDKEY_MASK               (0x00FFUL << WDT_CONFIG_WDKEY_POSI)
#endif // __2NDBOOT_SYS_H__

