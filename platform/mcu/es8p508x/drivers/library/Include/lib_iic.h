/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#ifndef __LIBIIC_H__
#define __LIBIIC_H__

#include "ES8P508x.h"
#include "type.h"
#include "system_ES8P508x.h"

typedef enum
{
    I2C_PinMode_PP = 0x0 , 
    I2C_PinMode_OD = 0x1 , 
}I2C_TYPE_PINOD;

typedef enum
{
    I2C_Mode_Master = 0x0 , 
    I2C_Mode_Slave = 0x1 ,  
}I2C_TYPE_MODE;

typedef enum
{
    I2C_AckDelay_0P5 = 0x0 , 
    I2C_AckDelay_1 = 0x1 ,   
    I2C_AckDelay_1P5 = 0x2 , 
    I2C_AckDelay_2 = 0x3 ,   
    I2C_AckDelay_2P5 = 0x4 , 
    I2C_AckDelay_3 = 0x5 ,   
    I2C_AckDelay_3P5 = 0x6 , 
    I2C_AckDelay_4 = 0x7 ,   
}I2C_TYPE_ADLY;

typedef enum
{
    I2C_RecMode_0 = 0x0 , 
    I2C_RecMode_1 = 0x1 , 
    I2C_RecMode_2 = 0x2 , 
    I2C_RecMode_3 = 0x3 , 
    I2C_RecMode_4 = 0x4 , 
    I2C_RecMode_5 = 0x5 , 
    I2C_RecMode_6 = 0x6,  
    I2C_RecMode_7 = 0x7,  
}I2C_TYPE_RECMODE;

typedef enum
{
    I2C_TI_Disable = 0x0 , 
    I2C_TI_1 = 0x1 ,       
    I2C_TI_2 = 0x2 ,       
    I2C_TI_3 = 0x3 ,       
    I2C_TI_4 = 0x4 ,       
    I2C_TI_5 = 0x5 ,       
    I2C_TI_6 = 0x6 ,      
    I2C_TI_7 = 0x7 ,      
    I2C_TI_8 = 0x8 ,      
    I2C_TI_9 = 0x9 ,    
    I2C_TI_10 = 0xA ,      
    I2C_TI_11 = 0xB ,      
    I2C_TI_12 = 0xC ,    
    I2C_TI_13 = 0xD ,      
    I2C_TI_14 = 0xE ,      
    I2C_TI_15 = 0xF ,      
}I2C_TYPE_TIS;

typedef struct
{
    I2C_TYPE_PINOD  I2C_SclOd; 

    I2C_TYPE_PINOD  I2C_SdaOd; 

    TYPE_FUNCEN  I2C_16XSamp;  

    uint32_t  I2C_Clk;         

    I2C_TYPE_MODE  I2C_Mode;   

    TYPE_FUNCEN  I2C_AutoStop; 

    TYPE_FUNCEN  I2C_AutoCall; 
}I2C_InitStruType;

typedef enum
{
    I2C_TRBIM_Byte = 0x0 ,     
    I2C_TRBIM_HalfWord = 0x1 , 
    I2C_TRBIM_Word = 0x2 ,     
}I2C_TYPE_TRBIM;

typedef enum
{
    I2C_Mode_Write = 0x0 , 
    I2C_Mode_Read = 0x1 ,
}I2C_TYPE_RWMODE;

typedef enum
{
    I2C_IT_SR = 0x0001 ,   
    I2C_IT_SP = 0x0002 ,   
    I2C_IT_TB = 0x0004 ,   
    I2C_IT_RB = 0x0008 ,   
    I2C_IT_TE = 0x0010 ,   
    I2C_IT_RO = 0x0020 ,  
    I2C_IT_NA = 0x0040 ,   
    I2C_IT_TBWE = 0x0080 , 
    I2C_IT_TIDLE = 0x01000 , 
}I2C_TYPE_IT;

typedef enum
{
    I2C_Flag_SR = 0x0001 ,    
    I2C_Flag_SP = 0x0002 ,    
    I2C_Flag_TB = 0x0004 ,    
    I2C_Flag_RB = 0x0008 ,    
    I2C_Flag_TE = 0x0010 ,   
    I2C_Flag_RO = 0x0020 ,   
    I2C_Flag_NA = 0x0040 ,    
    I2C_Flag_TBWE = 0x0080,  
    I2C_Flag_TIDLE= 0X1000,  
}I2C_TYPE_FLAG;

typedef enum
{
    I2C_Clr_SR = 0x0001 ,   
    I2C_Clr_SP = 0x0002 ,   
    I2C_Clr_TE = 0x0010 ,   
    I2C_Clr_RO = 0x0020 ,   
    I2C_Clr_NA = 0x0040 ,   
    I2C_Clr_TBWE = 0x0080 , 
    I2C_Clr_TIDLE= 0X01000,  
}I2C_CLR_IF;

#define I2C_Enable() (I2C0->CON.EN = 1)
#define I2C_Disable() (I2C0->CON.EN = 0)

#define I2C_Reset() (I2C0->CON.RST = 1)

#define I2C_TJEnable() (I2C0->CON.TJE = 1)
#define I2C_TJDisable() (I2C0->CON.TJE = 0)

#define I2C_Read() (I2C0->CON.RW = 1)
#define I2C_Write() (I2C0->CON.RW = 0)

#define I2C_CSEnable() (I2C0->MOD.CSE = 1)
#define I2C_CSDisable() (I2C0->MOD.CSE = 0)

#define I2C_ANAEnable() (I2C0->MOD.ANAE = 1)
#define I2C_ANADisable() (I2C0->MOD.ANAE = 0)

#define I2C_SRAEnable() (I2C0->MOD.SRAE = 1)
#define I2C_SRADisable() (I2C0->MOD.SRAE = 0)

#define I2C_SPAEnable() (I2C0->MOD.SPAE = 1)
#define I2C_SPADisable() (I2C0->MOD.SPAE = 0)

#define I2C_SRTrigger() (I2C0->MOD.SRT=1)

#define I2C_SPTrigger() (I2C0->MOD.SPT = 1)

#define I2C_RDTrigger() (I2C0->MOD.RDT = 1)

#define I2C_Release() (I2C0->MOD.BLD = 1)

#define I2C_TACK() (I2C0->MOD.TAS = 1)
#define I2C_TNACK() (I2C0->MOD.TAS = 0)


void I2C_Init(I2C_InitStruType* I2C_InitStruct);
void I2C_ITConfig(I2C_TYPE_IT I2C_IT,TYPE_FUNCEN NewState);
void I2C_SendAddress(uint8_t I2C_Address,I2C_TYPE_RWMODE Mode);
void I2C_SetAddress(uint8_t I2C_Address);
void I2C_RecModeConfig(I2C_TYPE_RECMODE RecType);
void I2C_TBIMConfig(I2C_TYPE_TRBIM Type);
void I2C_RBIMConfig(I2C_TYPE_TRBIM Type);
void I2C_AckDelay(I2C_TYPE_ADLY Type,TYPE_FUNCEN NewStatus);
void I2C_TISConfig(I2C_TYPE_TIS Time);
void I2C_SendByte(uint8_t Byte);
void I2C_SendHalfWord(uint16_t HalfWord);
void I2C_SendWord(uint32_t Word);
uint8_t I2C_RecByte(void);
uint16_t I2C_RecHalfWord(void);
uint32_t I2C_RecWord(void);
I2C_TYPE_RWMODE I2C_GetRWMode(void);
FlagStatus I2C_GetTBStatus(void);
FlagStatus I2C_GetFlagStatus(I2C_TYPE_FLAG I2C_Flag);
FlagStatus I2C_GetITStatus( I2C_TYPE_IT I2C_Flag);
void I2C_ClearITPendingBit(I2C_CLR_IF I2C_IT);

#endif

/*************************END OF FILE**********************/
