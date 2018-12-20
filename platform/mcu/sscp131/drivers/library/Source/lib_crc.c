/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_crc.h"
#include "type.h"

//typedef ErrorStatus (* pfunc_empty)(uint32_t Flash_Addr, uint32_t Len);
//typedef uint32_t (* pfunc_flash)(uint32_t Flash_Addr, uint32_t Len, uint32_t Cfg);
typedef uint32_t (* pfunc_user)(uint32_t *Addr, uint32_t Len, uint32_t Cfg);
typedef ErrorStatus (* pfunc_reset) ();

void CRC_DAMCmd(TYPE_FUNCEN NewState)
{
    if (NewState == ENABLE)        
       CRC->CON.DMAEN = 1;
    else
       CRC->CON.DMAEN = 0;   
}

void CRC_Init(CRC_InitStruType *CRC_InitStruct)
{
    CRC_RegUnLock();
    
    CRC->CON.EN = CRC_InitStruct->CRC_EnableCfg;
    CRC->CON.RST = CRC_InitStruct->CRC_Resect;
    CRC->CON.HS = CRC_InitStruct->CRC_HighEnable;
    CRC->CON.DS = CRC_InitStruct->CRC_DataStyle;
    CRC->CON.MOD = CRC_InitStruct->CRC_Mode;
    CRC->CON.BYTE = CRC_InitStruct->CRC_ByteWide;
    CRC->CON.REFIN = CRC_InitStruct->CRC_RefIn;
    CRC->CON.REFOUT = CRC_InitStruct->CRC_RefOut;
    CRC->CON.XOROUT = CRC_InitStruct->CRC_XorOut;
    
    CRC_RegLock();
}

uint32_t CRC_GetCRC(uint32_t *Addr, uint32_t Len)
{
    uint32_t Register_data;
    uint32_t CRC_data;
    
    Register_data = CRC->CON.Word;
    CRC_data = ((pfunc_user)(*(volatile unsigned int *)0x10008008))(Addr, Len, Register_data);    

    return CRC_data;
}

ErrorStatus CRC_CheckReset(void)
{
    ErrorStatus status;
    
    status = ((pfunc_reset)(*(volatile unsigned int *)0x1000800C))(); 
    
    return status;
}
