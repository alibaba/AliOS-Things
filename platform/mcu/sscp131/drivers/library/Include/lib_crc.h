/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_CRC_H__
#define __LIB_CRC_H__

#include "P131.h"
#include "type.h"

typedef enum
{
    CRC_ByteWide_Byte       = 0x0,
    CRC_ByteWide_HalfWord   = 0x1,
    CRC_ByteWide_Word       = 0x2,
} CRC_Type_ByteWide;

typedef enum
{
    CRC_Mode_CRC32  = 0x0,  //CRC32
    CRC_Mode_CRC24  = 0x1,  //CRC24
    CRC_Mode_CRC16  = 0x2,  //CRC16
    CRC_Mode_CRC16_CCITT = 0x3, //CRC16-CCITT
} CRC_Type_Mode;


#define CRC_RegUnLock() (CRC->UL.Word = 0x43524355)
#define CRC_RegLock()   (CRC->UL.Word = 0x00000000)

#define CRC_Enable      (CRC->CON.EN = 1)
#define CRC_Disable     (CRC->CON.EN = 0)

#define CRC_HSEnable    (CRC->CON.HS = 1)
#define CRC_HSDisable   (CRC->CON.HS = 0)


typedef struct {
    uint8_t CRC_EnableCfg;
    uint8_t CRC_Resect;
    uint8_t CRC_HighEnable;
    uint8_t CRC_DataStyle;
    CRC_Type_Mode       CRC_Mode;
    CRC_Type_ByteWide   CRC_ByteWide;
    uint8_t CRC_RefIn;
    uint8_t CRC_RefOut;
    uint8_t CRC_XorOut;
} CRC_InitStruType;

void CRC_Init(CRC_InitStruType *CRC_InitStruct);
void CRC_DAMCmd(TYPE_FUNCEN NewState);
uint32_t CRC_GetCRC(uint32_t *Addr, uint32_t Len);
ErrorStatus CRC_CheckReset(void);

#endif
