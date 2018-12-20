/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIBUART_H__
#define __LIBUART_H__

#include "p131.h"
#include "type.h"

typedef enum {
    UART_StopBits_1 = 0x0,
    UART_StopBits_2 = 0x1,
} UART_TYPE_TXFS;

typedef enum {
    UART_DataMode_7     = 0x4,
    UART_DataMode_8     = 0x0,
    UART_DataMode_9     = 0x2,
    UART_DataMode_7Odd  = 0xD,
    UART_DataMode_7Even = 0xC,
    UART_DataMode_7Add0 = 0xE,
    UART_DataMode_7Add1 = 0xF,
    UART_DataMode_8Odd  = 0x9,
    UART_DataMode_8Even = 0x8,
    UART_DataMode_8Add0 = 0xA,
    UART_DataMode_8Add1 = 0xB,
} UART_TYPE_DATAMOD;

typedef enum {
    UART_Polar_Normal   = 0x0,
    UART_Polar_Opposite = 0x1,
//    UART_Polar_Normal   = 0x1,
//  UART_Polar_Opposite = 0x0,
} UART_TYPE_RTXP;

typedef enum {
    UART_Clock_1 = 0x1,         //PCLK
    UART_Clock_2 = 0x2,         //PCLK / 2
    UART_Clock_3 = 0x3,         //PCLK / 4
    UART_Clock_4 = 0x4,         //PCLK / 8
} UART_TYPE_BCS;

typedef struct {
    UART_TYPE_TXFS    UART_StopBits;
    UART_TYPE_DATAMOD UART_TxMode;
    UART_TYPE_RTXP    UART_TxPolar;
    UART_TYPE_DATAMOD UART_RxMode;
    UART_TYPE_RTXP    UART_RxPolar;
    uint32_t          UART_BaudRate;
    UART_TYPE_BCS     UART_ClockSet;
} UART_InitStruType;

/* Interrupt */
typedef enum {
    UART_IT_TB   = (1 << 0),
    UART_IT_TC   = (1 << 1),
    UART_IT_TBWE = (1 << 8),
    UART_IT_TBWO = (1 << 9),
    UART_IT_RB   = (1 << 16),
    UART_IT_ID   = (1 << 17),
    UART_IT_RO   = (1 << 24),
    UART_IT_FE   = (1 << 25),
    UART_IT_PE   = (1 << 26),
    UART_IT_BDE  = (1 << 27),
    UART_IT_RBRE = (1 << 28),
    UART_IT_RBRO = (1 << 29),
} UART_TYPE_IT;

/* Interrupt Flag */
typedef enum {
    UART_FLAG_TB   = (1 << 0),
    UART_FLAG_TC   = (1 << 1),
    UART_FLAG_TBWE = (1 << 8),
    UART_FLAG_TBWO = (1 << 9),
    UART_FLAG_RB   = (1 << 16),
    UART_FLAG_ID   = (1 << 17),
    UART_FLAG_RO   = (1 << 24),
    UART_FLAG_FE   = (1 << 25),
    UART_FLAG_PE   = (1 << 26),
    UART_FLAG_BDE  = (1 << 27),
    UART_FLAG_RBRE = (1 << 28),
    UART_FLAG_RBRO = (1 << 29),
} UART_TYPE_FLAG;

/* Interrupt Clear Flag */
typedef enum {
    UART_CLR_TC   = (1 << 1),
    UART_CLR_TBWE = (1 << 8),
    UART_CLR_TBWO = (1 << 9),
    UART_CLR_ID   = (1 << 17),
    UART_CLR_RO   = (1 << 24),
    UART_CLR_FE   = (1 << 25),
    UART_CLR_PE   = (1 << 26),
    UART_CLR_BDE  = (1 << 27),
    UART_CLR_RBRE = (1 << 28),
    UART_CLR_RBRO = (1 << 29),
} UART_CLR_IF;

/* Interrupt Mode */
typedef enum {
    UART_TBIM_Byte     = 0x0,
    UART_TBIM_HalfWord = 0x1,
    UART_TBIM_Word     = 0x2,
    UART_TBIM_Full     = 0x3,
} UART_TYPE_TRBIM;

/* Status */
typedef enum {
    UART_STA_TBOV   = (1 << 4),
    UART_STA_TXBUSY = (1 << 5),
    UART_STA_RBOV   = (1 << 12),
    UART_STA_RXBUSY = (1 << 13),
    UART_STA_FER0   = (1 << 16),
    UART_STA_PER0   = (1 << 17),
    UART_STA_FER1   = (1 << 18),
    UART_STA_PER1   = (1 << 19),
    UART_STA_FER2   = (1 << 20),
    UART_STA_PER2   = (1 << 21),
    UART_STA_FER3   = (1 << 22),
    UART_STA_PER3   = (1 << 23),
} UART_TYPE_STA;

#define UART0_TxEnable()    (UART0->CON0.TXEN = 1)
#define UART1_TxEnable()    (UART1->CON0.TXEN = 1)
#define UART0_TxDisable()   (UART0->CON0.TXEN = 0)
#define UART1_TxDisable()   (UART1->CON0.TXEN = 0)
#define UART0_RxEnable()    (UART0->CON0.RXEN = 1)
#define UART1_RxEnable()    (UART1->CON0.RXEN = 1)
#define UART0_RxDisable()   (UART0->CON0.RXEN = 0)
#define UART1_RxDisable()   (UART1->CON0.RXEN = 0)

#define UART2_TxEnable()    (UART2->CON0.TXEN = 1)
#define UART3_TxEnable()    (UART3->CON0.TXEN = 1)
#define UART2_TxDisable()   (UART2->CON0.TXEN = 0)
#define UART3_TxDisable()   (UART3->CON0.TXEN = 0)
#define UART2_RxEnable()    (UART2->CON0.RXEN = 1)
#define UART3_RxEnable()    (UART3->CON0.RXEN = 1)
#define UART2_RxDisable()   (UART2->CON0.RXEN = 0)
#define UART3_RxDisable()   (UART3->CON0.RXEN = 0)

#define UART0_TxRst()       (UART0->CON0.TRST = 1)
#define UART1_TxRst()       (UART1->CON0.TRST = 1)
#define UART0_RxRst()       (UART0->CON0.RRST = 1)
#define UART1_RxRst()       (UART1->CON0.RRST = 1)

#define UART2_TxRst()       (UART2->CON0.TRST = 1)
#define UART3_TxRst()       (UART3->CON0.TRST = 1)
#define UART2_RxRst()       (UART2->CON0.RRST = 1)
#define UART3_RxRst()       (UART3->CON0.RRST = 1)


/*  Function */
void UART_Init(UART_TypeDef *UARTx, UART_InitStruType *UART_InitStruct);
void UART_ITConfig(UART_TypeDef *UARTx, UART_TYPE_IT UART_IT, TYPE_FUNCEN NewState);
void UART_TBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type);
void UART_RBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type);
void UART_SendByte(UART_TypeDef *UARTx, uint8_t data08);
void UART_SendHalfWord(UART_TypeDef *UARTx, uint16_t data16);
void UART_SendWord(UART_TypeDef *UARTx, uint32_t data32);
uint8_t UART_RecByte(UART_TypeDef *UARTx);
uint16_t UART_RecHalfWord(UART_TypeDef *UARTx);
uint32_t UART_RecWord(UART_TypeDef *UARTx);
FlagStatus UART_GetStatus(UART_TypeDef *UARTx, UART_TYPE_STA UART_Flag);
FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, UART_TYPE_FLAG UART_Flag);
ITStatus UART_GetITStatus(UART_TypeDef *UARTx, UART_TYPE_IT UART_Flag);
void UART_ClearITPendingBit(UART_TypeDef *UARTx, UART_CLR_IF UART_Flag);
void UART_ConfigDMAONERR(UART_TypeDef *UARTx,TYPE_FUNCEN state);
void UART_ConfigRXDMAEN(UART_TypeDef *UARTx,TYPE_FUNCEN state);
void UART_ConfigTXDMAEN(UART_TypeDef *UARTx,TYPE_FUNCEN state);

#endif
