/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 *
 */
#ifndef _EFLASH_PRIV_H_
#define _EFLASH_PRIV_H_

#define SECTOR_SIZE 512

/* E FLASH */
#define FLASH_CTRL_BASEADDR                     0x4003F000

#define FLASH_ADDR_OFS                          0x04
#define FLASH_PE_OFS                            0x10
#define FLASH_WE_OFS                            0x18
#define FLASH_WDATA_OFS                         0x1C

#endif
