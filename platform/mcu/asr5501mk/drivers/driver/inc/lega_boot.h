#ifndef _LEGA_BOOT_H_
#define _LEGA_BOOT_H_

#define AON_RST_CHECK_REG       (ALWAYS_ON_REGFILE + 0x10)
#define AON_RST_CHECK_FLAG      0x00000002
#define LEGA_AON_POWER_ON_FLAG  0x12345678

#define PWR_ON_RST              0
#define HARDWARE_PIN_RST        1
#define SOFTWARE_RST            2 //including NVIC_SystemReset and WDG RST

uint8_t lega_get_boot_type(void);
#endif //_LEGA_BOOT_H_