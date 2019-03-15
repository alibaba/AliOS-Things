/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/

#ifndef __LIBU7816_H__
#define __LIBU7816_H__

#include "HR8P2xx.h"

#include "lib_uart.h"
#include "type.h"

typedef enum
{
    EUART_StopBits_1 = 0x0,
    EUART_StopBits_2 = 0x1,
} EUART_TYPE_TXFS;

typedef enum
{
    EUART_DataMode_7     = 0x0,
    EUART_DataMode_8     = 0x1,
    EUART_DataMode_9     = 0x2,
    EUART_DataMode_7Odd  = 0x4,
    EUART_DataMode_7Even = 0x5,
    EUART_DataMode_8Odd  = 0x6,
    EUART_DataMode_8Even = 0x7,
} EUART_TYPE_DATAMOD;

typedef enum
{
    EUART_Polar_Normal   = 0x0,
    EUART_Polar_Opposite = 0x1,
} EUART_TYPE_RTXP;

typedef enum
{
    EUART_Clock_1 = 0x1,
    EUART_Clock_2 = 0x2,
    EUART_Clock_3 = 0x3,
} EUART_TYPE_BCS;

typedef struct
{
    EUART_TYPE_TXFS  EUART_StopBits;

    EUART_TYPE_DATAMOD  EUART_TxMode;

    EUART_TYPE_RTXP  EUART_TxPolar;

    EUART_TYPE_DATAMOD  EUART_RxMode;

    EUART_TYPE_RTXP  EUART_RxPolar;

    uint32_t  EUART_BaudRate;

    EUART_TYPE_BCS  EUART_ClockSet;

} EUART_InitStruType;

typedef enum
{
    EUART_IT_TB     = 0x0001,
    EUART_IT_RB     = 0x0002,
    EUART_IT_RO     = 0x0004,
    EUART_IT_FE     = 0x0008,
    EUART_IT_PE     = 0x0010,
    EUART_IT_TBWE   = 0x0020,
    EUART_IT_ARTE   = 0x0040,
    EUART_IT_RNA    = 0x0080,
    EUART_IT_TXIDLE = 0x0100,
    EUART_IT_RXIDLE = 0x0200,
} EUART_TYPE_IT;

typedef enum
{
    EUART_FLAG_TB     = 0x0001,
    EUART_FLAG_RB     = 0x0002,
    EUART_FLAG_RO     = 0x0004,
    EUART_FLAG_FE     = 0x0008,
    EUART_FLAG_PE     = 0x0010,
    EUART_FLAG_TBWE   = 0x0020,
    EUART_FLAG_ARTE   = 0x0040,
    EUART_FLAG_RNA    = 0x0080,
    EUART_FLAG_TXIDLE = 0x0100,
    EUART_FLAG_RXIDLE = 0x0200,
} EUART_TYPE_FLAG;

typedef enum
{
    EUART_Clr_RO   = 0x0004,
    EUART_Clr_FE   = 0x0008,
    EUART_Clr_PE   = 0x0010,
    EUART_Clr_TBWE = 0x0020,
    EUART_Clr_ARTE = 0x0040,
    EUART_Clr_RNA  = 0x0080,
} EUART_CLR_IF;

typedef enum
{
    EUART_TRBIM_Byte     = 0x0,
    EUART_TRBIM_HalfWord = 0x1,
    EUART_TRBIM_Word     = 0x2,
} EUART_TYPE_TRBIM;

typedef enum
{
    EUART_Mode_U7816 = 0x1,
    EUART_Mode_Uart  = 0x0,
} EUART_TYPE_MODE;

typedef enum
{
    U7816_CHS_EIO0 = 0x0,
    U7816_CHS_EIO1 = 0x1,
} U7816_TYPE_CHS;

typedef enum
{
    U7816_EIODir_In  = 0x0,
    U7816_EIODir_Out = 0x1,
} U7816_TYPE_EIOC;

typedef enum
{
    U7816_DataForm_Normal   = 0x0,
    U7816_DataForm_Contrary = 0x1,
} U7816_TYPE_DAS;

typedef enum
{
    U7816_Verify_Odd  = 0x0,
    U7816_Verify_Even = 0x1,
} U7816_TYPE_PS;

typedef enum
{
    U7816_NACKWidth_1ETU   = 0x0,
    U7816_NACKWidth_1P5ETU = 0x1,
    U7816_NACKWidth_2ETU   = 0x2,
} U7816_TYPE_TNAS;

typedef enum
{
    U7816_RetryTimes_1 = 0x0,
    U7816_RetryTimes_2 = 0x1,
    U7816_RetryTimes_3 = 0x2,
    U7816_RetryTimes_N = 0x3,
} U7816_TYPE_ARTS;

typedef enum
{
    U7816_PCLK_1 = 0x0,
    U7816_PCLK_2 = 0x1,
    U7816_PCLK_4 = 0x2,
    U7816_PCLK_8 = 0x3,
} U7816_TYPE_CKS;

typedef struct
{    
    EUART_TYPE_BCS  U7816_ClockSet;

    uint32_t  U7816_BaudRate;

    TYPE_FUNCEN  U7816_ECK0;

    TYPE_FUNCEN  U7816_ECK1;

    U7816_TYPE_CHS  U7816_EIOCh;

    U7816_TYPE_EIOC  U7816_EIODir;

    U7816_TYPE_DAS  U7816_DataForm;

    U7816_TYPE_PS  U7816_DataVerify;

    TYPE_FUNCEN  U7816_AutoRetryTX;

    TYPE_FUNCEN  U7816_AutoRetryRX;

    U7816_TYPE_TNAS  U7816_NACK_Width;

    U7816_TYPE_ARTS  U7816_RetryTimes;

    U7816_TYPE_CKS  U7816_CLKS;

    uint8_t  U7816_ETUTime;

} U7816_InitStruType;

#define EUART0_TxEnable()  (EUART0->CON.TXEN = 1)
#define EUART1_TxEnable()  (EUART1->CON.TXEN = 1)

#define EUART0_TxDisable() (EUART0->CON.TXEN = 0)
#define EUART1_TxDisable() (EUART1->CON.TXEN = 0)

#define EUART0_RxEnable()  (EUART0->CON.RXEN = 1)
#define EUART1_RxEnable()  (EUART1->CON.RXEN = 1)

#define EUART0_RxDisable() (EUART0->CON.RXEN = 0)
#define EUART1_RxDisable() (EUART1->CON.RXEN = 0)

#define EUART0_TxRst() (EUART0->CON.TRST = 1)
#define EUART1_TxRst() (EUART1->CON.TRST = 1)

#define EUART0_RxRst() (EUART0->CON.RRST = 1)
#define EUART1_RxRst() (EUART1->CON.RRST = 1)

#define EUART0_U7816_REST() ( EUART0->MOD.ERST = 1)
#define EUART1_U7816_REST() ( EUART1->MOD.ERST = 1)

void EUART_ModeConfig(EUART_TypeDef* EUARTx,EUART_TYPE_MODE Mode);
void EUART_Init(EUART_TypeDef* EUARTx,EUART_InitStruType* EUART_InitStruct);
void EUART_BaudConfig(EUART_TypeDef* EUARTx, uint32_t BaudRate, EUART_TYPE_BCS ClockSet);
void EUART_ITConfig(EUART_TypeDef* EUARTx, EUART_TYPE_IT EUART_IT, TYPE_FUNCEN NewState);
void EUART_TBIMConfig(EUART_TypeDef* EUARTx,EUART_TYPE_TRBIM Type);
void EUART_RBIMConfig(EUART_TypeDef* EUARTx,EUART_TYPE_TRBIM Type);
FlagStatus EUART_GetFlagStatus(EUART_TypeDef* EUARTx,EUART_TYPE_FLAG EUART_Flag);
ITStatus EUART_GetITStatus(EUART_TypeDef* EUARTx,EUART_TYPE_IT EUART_Flag);
void EUART_ClearITPendingBit(EUART_TypeDef* EUARTx,EUART_CLR_IF EUART_Flag);

void U7816_Init(EUART_TypeDef* EUARTx, U7816_InitStruType* U7816_InitStruct);
void EUART_EIOChConfig(EUART_TypeDef* EUARTx, U7816_TYPE_CHS U7816_IO);
void EUART_EIODirection(EUART_TypeDef* EUARTx, U7816_TYPE_EIOC Dir);
void EUART_SendByte(EUART_TypeDef* EUARTx, uint8_t ByteData);
void EUART_SendHalfWord(EUART_TypeDef* EUARTx, uint16_t HalfWordData);
void EUART_SendWord(EUART_TypeDef* EUARTx, uint32_t WordData);
uint8_t EUART_RecByte(EUART_TypeDef* EUARTx);
uint16_t EUART_RecHalfWord(EUART_TypeDef* EUARTx);
uint32_t EUART_RecWord(EUART_TypeDef* EUARTx);

#endif

/*************************END OF FILE**********************/
