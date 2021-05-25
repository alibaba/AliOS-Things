/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
 
#ifndef EX_API_H
#define EX_API_H

#include "device.h"
#include "serial_api.h"
#include "spi_api.h"
#include "dma_api.h"
#include "flash_api.h"
#include "gpio_ex_api.h"
#include "gpio_irq_ex_api.h"
#include "i2c_ex_api.h"
#include "i2s_api.h"
#include "serial_ex_api.h"
#include "sleep_ex_api.h"
#include "spi_ex_api.h"
#include "sys_api.h"
#include "wdt_api.h"

#if defined(CONFIG_PLATFORM_8195A) && (CONFIG_PLATFORM_8195A == 1)
#include "nfc_api.h"
#include "ethernet_ex_api.h"
#endif //CONFIG_PLATFORM_8195A

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif

