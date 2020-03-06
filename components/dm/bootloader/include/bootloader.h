/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H
#include "aos/hal/flash.h"
#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"
#include "updater.h"

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 0x1000 /* flash sector size */
#endif

#ifndef OTA_2NDBOOT_WDG_TIMEOUT
#define OTA_2NDBOOT_WDG_TIMEOUT 120
#endif

#define UART_DEFAULT_TIMEOUT 1000

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FLASH_ALIGN_MASK ~(sizeof(unsigned int) - 1)
#define FLASH_ALIGN sizeof(unsigned int)

/* system APIs */
void sys_reboot(void);
void sys_delayms(volatile int ms);

/* uart APIs */
int uart_init(void);
void uart_send_string(char *buf);
int uart_recv_byte(unsigned char *c);
void uart_send_byte(unsigned char buf);

/* flash APIs */
int flash_get_partition(uint32_t addr, uint32_t len, hal_partition_t* no, uint32_t* offset);
int flash_erase(unsigned int offset, unsigned int len);
int flash_read_data(unsigned int offset, unsigned char *buf, unsigned int len);
int flash_write_data(unsigned int offset, unsigned char *buf, unsigned int len);
hal_logic_partition_t *flash_get_info(hal_partition_t no);

/* watchdog APIs */
void boot_wdg_init(unsigned int ms);
void boot_wdg_reload(void);
void boot_wdg_finalize(void);

void boot_reset(void);
void boot_delay(unsigned int ms);

#endif /* _BOOTLOADER_H */
