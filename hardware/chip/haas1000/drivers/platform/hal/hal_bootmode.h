/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_BOOTMODE_H__
#define __HAL_BOOTMODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define HAL_HW_BOOTMODE_MASK                    (0xF << 0)
#define HAL_SW_BOOTMODE_MASK                    (0x0FFFFFFF << 4)

// SW_BOOTMODE_START                            (1 << 4)
#define HAL_SW_BOOTMODE_READ_ENABLED            (1 << 4)
#define HAL_SW_BOOTMODE_WRITE_ENABLED           (1 << 5)
#define HAL_SW_BOOTMODE_JTAG_ENABLED            (1 << 6)
#define HAL_SW_BOOTMODE_FORCE_USB_DLD           (1 << 7)
#define HAL_SW_BOOTMODE_FORCE_UART_DLD          (1 << 8)
#define HAL_SW_BOOTMODE_DLD_TRANS_UART          (1 << 9)
#define HAL_SW_BOOTMODE_SKIP_FLASH_BOOT         (1 << 10)
#define HAL_SW_BOOTMODE_CHIP_TEST               (1 << 11)
#define HAL_SW_BOOTMODE_FACTORY                 (1 << 12)
#define HAL_SW_BOOTMODE_CALIB                   (1 << 13)
#define HAL_SW_BOOTMODE_ROM_RESERVED_14         (1 << 14)
#define HAL_SW_BOOTMODE_FLASH_BOOT              (1 << 15)
#define HAL_SW_BOOTMODE_REBOOT                  (1 << 16)
#define HAL_SW_BOOTMODE_ROM_RESERVED_17         (1 << 17)
#define HAL_SW_BOOTMODE_FORCE_USB_PLUG_IN       (1 << 18)
#define HAL_SW_BOOTMODE_POWER_DOWN_WAKEUP       (1 << 19)

// APP_BOOTMODE_START                           (1 << 20)
// Add new application bootmodes here (from highest bit to lowest bit)

#define HAL_SW_BOOTMODE_TEST_MASK               (0x1F << 20)
#define HAL_SW_BOOTMODE_TEST_MODE               (1 << 20)
#define HAL_SW_BOOTMODE_TEST_SIGNALINGMODE      (1 << 21)
#define HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE    (1 << 22)
#define HAL_SW_BOOTMODE_TEST_SIGNALINGMODE_WIFI (1 << 23)
#define HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI (1 << 24)

#define HAL_SW_BOOTMODE_ENTER_HIDE_BOOT         (1 << 25)

#define HAL_SW_BOOTMODE_RESERVED_BIT24          (1 << 26)
#define HAL_SW_BOOTMODE_REBOOT_FROM_CRASH       (1 << 27)

#ifdef __USB_COMM__
#define HAL_SW_BOOTMODE_CDC_COMM                (1 << 28)
#endif
#ifdef ANC_APP
#define HAL_SW_BOOTMODE_REBOOT_BT_ON            (1 << 29)
#define HAL_SW_BOOTMODE_REBOOT_ANC_ON           (1 << 30)
#endif

#define HAL_SW_BOOTMODE_WARM_BOOT               (1 << 29)

#ifdef USER_REBOOT_PLAY_MUSIC_AUTO
#define HAL_SW_BOOTMODE_LOCAL_PLAYER            (1 << 31)
#endif

// APP_BOOTMODE_END                             (1 << 31)
// SW_BOOTMODE_END                              (1 << 31)

union HAL_HW_BOOTMODE_T {
    struct {
        uint8_t watchdog    : 1;
        uint8_t global      : 1;
        uint8_t rtc         : 1;
        uint8_t charger     : 1;
    };
    uint8_t reg;
};

union HAL_HW_BOOTMODE_T hal_rom_hw_bootmode_get(void);

union HAL_HW_BOOTMODE_T hal_hw_bootmode_get(void);

void hal_hw_bootmode_init(void);

void hal_hw_bootmode_clear(uint8_t clear_bits);

uint32_t hal_sw_bootmode_get(void);

void hal_sw_bootmode_set(uint32_t bm);

void hal_sw_bootmode_clear(uint32_t bm);

#ifdef __cplusplus
}
#endif

#endif

