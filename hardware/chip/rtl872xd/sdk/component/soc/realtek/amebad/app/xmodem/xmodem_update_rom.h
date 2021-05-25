/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _FW_UPDATE_ROM_H_
#define _FW_UPDATE_ROM_H_

#include "ameba_soc.h"

#define BOOT_UART_IDX           0
#define BOOT_UART_PIN_MUX       1
#define BOOT_UART_BAUD_RATE     38400
#define BOOT_UART_MAX_IMG_SZ    (32*1024*1024)      // 32M

extern VOID xmodem_img_write(char *ptr,  unsigned int wr_offset, unsigned int frame_size);
extern VOID xmodem_img_download(u8 uart_idx);

#endif  // end of "#define _FW_UPDATE_ROM_H_"

