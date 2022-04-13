/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef SECOND_BOOT_HAL_H
#define SECOND_BOOT_HAL_H
#include "aos/hal/flash.h"
#include "updater.h"

#define SECTOR_SIZE 0x1000    /* flash sector size */

#ifndef OTA_2NDBOOT_WDG_TIMEOUT
#define OTA_2NDBOOT_WDG_TIMEOUT 120
#endif

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FLASH_ALIGN_MASK ~(sizeof(unsigned int) - 1)
#define FLASH_ALIGN sizeof(unsigned int)

/* system APIs */
void sys_reboot(void);
void sys_delayms(volatile int ms);
#ifdef AOS_2ND_BOOT_NO_LDS
void *sys_set_heap(unsigned int *size);
#endif

/* uart APIs */
void uart_init(void);
void uart_send(unsigned char *buf, int len);
void uart_send_string(char *buf);
unsigned char uart_recv_byte(unsigned char *c);
void uart_send_byte(unsigned char buf);

/* flash APIs */
int flash_erase(unsigned int offset, unsigned int len);
int flash_read_data(unsigned int offset, unsigned char *buf, unsigned int len);
int flash_write_data(unsigned int offset, unsigned char *buf, unsigned int len);
hal_logic_partition_t *flash_get_info(hal_partition_t no);

/* watchdog APIs */
void wdg_init(unsigned int ms);
void wdg_feed(void);
void wdg_finish(void);

#ifdef AOS_2ND_BOOT_AB
/* 0: A, 1: B*/
int ota_2ndboot_ab_get(void);
void ota_2ndboot_ab_switch(void);
#endif

#endif /*SECOND_BOOT_HAL_H */
