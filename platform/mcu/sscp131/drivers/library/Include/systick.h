/*
 ****************************************************************************************
 *
 * @file    systick.h
 * @brief   systick module.
 *
 * @version V1.0
 * @date    03 Jan 2017
 * @author  zwtian
 * @note
 *
 * Copyright (C) Shanghai Eastsoft Microelectronics Co. Ltd. All rights reserved.
 *
 ****************************************************************************************
 */

#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "type.h"
extern volatile uint32_t g_ui32Flags;
#define SYS_SECOND          0x00000001
#define COMMAND_RECEIVED    0x00000002



extern uint32_t g_syscnt;

extern void systick_init(void);
extern void delay_10ms(uint32_t n);
extern void delay_us(uint32_t n);

uint32_t get_sys_tick(void);
#endif
