/*
 ****************************************************************************************
 *
 * @file    lib_wdt.h
 * @brief   library of WDT Module.
 *
 * @version V1.0
 * @date    22 Jan 2017
 * @author  ESM Application Team
 * @note
 *
 * Copyright (C) Shanghai Eastsoft Microelectronics Co. Ltd. All rights reserved.
 *
 ****************************************************************************************
 */
#ifndef __LIB_WDT_H__
#define __LIB_WDT_H__

#include "type.h"
#include "p131.h"

typedef enum {
    WWDT_WIN_25  = 0x00,
    WWDT_WIN_50  = 0x01,
    WWDT_WIN_75  = 0x02,
    WWDT_WIN_100 = 0x03,
} WWDT_WIN;

void WWDT_Init(uint32_t load, uint8_t win, uint8_t it_flag);
void WWDT_ClearFlag(void);
FlagStatus WWDT_GetFlagStatus(void);
uint32_t WWDT_GetValue(void);

void IWDT_Init(uint32_t load, uint8_t it_flag);
void IWDT_ClearFlag(void);
FlagStatus IWDT_GetFlagStatus(void);
uint32_t IWDT_GetValue(void);

#define WWDT_Unlock()   {WWDT->LOCK.Word = 0x1ACCE551;}
#define WWDT_Lock() {WWDT->LOCK.Word = 0xFFFFFFFF;}

#define IWDT_Unlock()   {IWDT->LOCK.Word = 0x1ACCE551;}
#define IWDT_Lock() {IWDT->LOCK.Word = 0xFFFFFFFF;}

#endif
