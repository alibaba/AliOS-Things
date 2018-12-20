/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#ifndef __LIB_FLASHIAP_H__
#define __LIB_FLASHIAP_H__

#include "P131.h"
#include "type.h"

typedef enum
{
    IAP_Clock_25000K    = 0x0,
    IAP_Clock_50000K    = 0x1,
    IAP_Clock_100000K   = 0x2,
    IAP_Clock_12500K    = 0x3,
    IAP_Clock_18750K    = 0x4,
    IAP_Clock_37500K    = 0x5,
    IAP_Clock_75000K    = 0x6,
    IAP_Clock_6250K     = 0x7,
} IAP_Clock;

typedef enum
{
    IAP_PageErase_Erase  = 1,
    IAP_PageErase_Retain = 0,
} IAP_PageErase;

typedef ErrorStatus (* pfunc_erase)(uint32_t Flash_Addr, IAP_Clock clock);
typedef ErrorStatus (* pfunc_word)(uint32_t Flash_Addr, uint32_t *data, IAP_Clock clock);
typedef ErrorStatus (* pfunc_page)(uint32_t Flash_Addr, uint32_t *data, uint32_t len, IAP_PageErase pageErase, IAP_Clock clock);

#define IAP_PageErase(Flash_Addr, clock)        ((pfunc_erase)(*(volatile unsigned int *)0x10000004))(Flash_Addr, clock)
#define IAP_WordProgran(Flash_Addr, data, clock)    ((pfunc_word)(*(volatile unsigned int *)0x10000008))(Flash_Addr, data, clock)
#define IAP_WordsProgram(Flash_Addr, data, len, pageErase, clock) \
                                ((pfunc_page)(*(volatile unsigned int *)0x10000000))(Flash_Addr, data, len, pageErase, clock)

ErrorStatus IAP_Read(uint32_t * Ram_Addr, uint32_t Flash_Addr,  uint8_t Len);                                
#endif
