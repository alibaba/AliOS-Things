/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#ifndef __LIBSPI_H__
#define __LIBSPI_H__

#include "system_ES8P508x.h"
#include "ES8P508x.h"
#include "type.h"

typedef enum
{
    SPI_RiseSendFallRec = 0x0 , 
    SPI_FallSendRiseRec = 0x1 , 
    SPI_RiseRecFallSend = 0x2 , 
    SPI_FallRecRiseSend = 0x3 , 
}SPI_TYPE_DFS;

typedef enum
{
    SPI_Mode_Master = 0x0 , 
    SPI_Mode_Slave = 0x1 ,  
}SPI_TYPE_MODE;

typedef struct
{
    uint32_t  SPI_Freq;          

    SPI_TYPE_DFS  SPI_Df;        

    SPI_TYPE_MODE  SPI_Mode;     

    uint8_t SPI_DW;             

    TYPE_FUNCEN  SPI_DelayRec;   

    TYPE_FUNCEN  SPI_DelaySend;  

    uint8_t  SPI_SendDelayPeroid;

}SPI_InitStruType;

typedef enum
{
    SPI_Flag_TB = 0x00000001,
    SPI_Flag_RB = 0x00000002,
    SPI_Flag_TE = 0x00000004,
    SPI_Flag_RO = 0x00000008,
    SPI_Flag_ID = 0x00000010,
    SPI_Flag_NSSIF = 0x00000020,
    SPI_Flag_TBWE = 0x00000040,
    SPI_Flag_NSS = 0x00000080,
    SPI_Flag_TBEF0 = 0x00000100,
    SPI_Flag_TBEF1 = 0x00000200,
    SPI_Flag_TBEF2 = 0x00000400,
    SPI_Flag_TBEF3 = 0x00000800,
    SPI_Flag_RBFF0 = 0x00001000,
    SPI_Flag_RBFF1 = 0x00002000,
    SPI_Flag_RBFF2 = 0x00004000,
    SPI_Flag_RBFF3 = 0x00008000,
    SPI_Flag_IDLE = 0x00010000,
    SPI_Flag_TMS = 0x00020000,
}SPI_TYPE_FLAG;

typedef enum
{
    SPI_IType_BYTE = 0x0 ,
    SPI_IType_HALFWORD = 0x1 ,
    SPI_IType_WORD = 0x2 ,
}SPI_TYPE_TRBIM;


typedef enum
{
    SPI_IT_TB = 0x01 ,
    SPI_IT_RB = 0x02 ,
    SPI_IT_TE = 0x04 ,
    SPI_IT_RO = 0x08 ,
    SPI_IT_ID = 0x10 ,
    SPI_IT_NSS = 0x20 ,
    SPI_IT_TBWE = 0x40 ,
}SPI_TYPE_IT;


typedef enum
{
    SPI_Clr_TE = 0x04 ,
    SPI_Clr_RO = 0x08 ,
    SPI_Clr_ID = 0x10 ,
    SPI_Clr_NSS = 0x20 ,
    SPI_Clr_TBWE = 0x40 ,
}SPI_CLR_IF;




#define SPI_Enable()  (SPI0->CON.EN = 1)
#define SPI_Disable() (SPI0->CON.EN = 0)


#define SPI_RecEnable()  (SPI0->CON.REN = 1)
#define SPI_RecDisable() (SPI0->CON.REN = 0)


#define SPI_Rst() (SPI0->CON.RST = 1)


#define SPI_RX_CLR() (SPI0->CON.RXCLR = 1)
#define SPI_TX_CLR() (SPI0->CON.TXCLR = 1)


void SPI_Init(SPI_InitStruType* SPI_InitStruct);
void SPI_ITConfig(SPI_TYPE_IT SPI_IE,TYPE_FUNCEN NewState);
void SPI_DataFormatConfig(SPI_TYPE_DFS Type);
void SPI_SendByte(uint8_t Temp);
void SPI_SendHalfWord(uint16_t Temp);
void SPI_SendWord(uint32_t Temp);
uint8_t SPI_RecByte(void);
uint16_t SPI_RecHalfWord(void);
uint32_t SPI_RecWord(void);
void SPI_TBIMConfig(SPI_TYPE_TRBIM Type);
void SPI_RBIMConfig(SPI_TYPE_TRBIM Type);
FlagStatus SPI_GetFlagStatus(SPI_TYPE_FLAG Flag);
ITStatus SPI_GetITStatus(SPI_TYPE_IT Flag);
void SPI_ClearITPendingBit(SPI_CLR_IF Flag);

#endif

/*************************END OF FILE**********************/
