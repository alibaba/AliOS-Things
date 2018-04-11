/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_uart.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  UART模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBUART_H__
#define __LIBUART_H__

#include "HR8P506.h"
#include "type.h"

/* 停止位 */
typedef enum {
	UART_StopBits_1 = 0x0,		//1位停止位
	UART_StopBits_2 = 0x1,		//2位停止位
} UART_TYPE_TXFS;

/* 数据格式 */
typedef enum {
	UART_DataMode_7     = 0x4,	//7位数据
	UART_DataMode_8     = 0x0,	//8位数据
	UART_DataMode_9     = 0x2,	//9位数据
	UART_DataMode_7Odd  = 0xD,	//7位数据＋奇校验
	UART_DataMode_7Even = 0xC,	//7位数据＋偶校验
	UART_DataMode_7Add0 = 0xE,	//7位数据＋固定0
	UART_DataMode_7Add1 = 0xF,	//7位数据＋固定1 
	UART_DataMode_8Odd  = 0x9,	//8位数据＋奇校验
	UART_DataMode_8Even = 0x8,	//8位数据＋偶校验
	UART_DataMode_8Add0 = 0xA,	//8位数据＋固定0
	UART_DataMode_8Add1 = 0xB,	//8位数据＋固定1
} UART_TYPE_DATAMOD;

/* 端口极性 */
typedef enum {
	UART_Polar_Normal   = 0x0,	//正极性
	UART_Polar_Opposite = 0x1,	//负极性
} UART_TYPE_RTXP;

/* 时钟选择 */
typedef enum {
	UART_Clock_1 = 0x1,			//PCLK
	UART_Clock_2 = 0x2,			//PCLK / 2
	UART_Clock_3 = 0x3,			//PCLK / 4
	UART_Clock_4 = 0x4,			//PCLK / 8
} UART_TYPE_BCS;

/* 初始化结构体 */
typedef struct {
	UART_TYPE_TXFS    UART_StopBits;	//停止位选择
	UART_TYPE_DATAMOD UART_TxMode;		//发送数据格式
	UART_TYPE_RTXP    UART_TxPolar;		//发送端口极性
	UART_TYPE_DATAMOD UART_RxMode;		//接收数据格式
	UART_TYPE_RTXP    UART_RxPolar;		//接收端口极性
	uint32_t          UART_BaudRate;	//波特率
	UART_TYPE_BCS     UART_ClockSet;	//UART时钟选择
} UART_InitStruType;

/*自动波特率检测模式*/
typedef enum {
	UART_AUTO_BAUD_MODE1 = 0x00,	//自动波特率检测模式1,接收数据流以二进制1开始
	UART_AUTO_BAUD_MODE2 = 0x01,	//自动波特率检测模式1,接收数据流以二进制10开始
	UART_AUTO_BAUD_MODE3 = 0x02,	//自动波特率检测模式1,接收数据流以二进制1111_1110开始
	UART_AUTO_BAUD_MODE4 = 0x03,	//自动波特率检测模式1,接收数据流以二进制1010_1010开始
} UART_AUTO_BAUD_MODE;

/*空闲检测模式*/
typedef enum {
	UART_AUTO_BAUD_FREE_MODE1 = 0x00,	//将检测连续 10 个比特位空闲
	UART_AUTO_BAUD_FREE_MODE2 = 0x01,	//将检测连续 11 个比特位空闲
	UART_AUTO_BAUD_FREE_MODE3 = 0x02,	//将检测连续 12 个比特位空闲
    UART_AUTO_BAUD_FREE_MODE4 = 0x03,	//将检测连续 13 个比特位空闲
} UART_AUTO_BAUD_FREE_MODE;

/* Interrupt */
typedef enum {
	UART_IT_TB   = (1 << 0),		//发送缓冲器空中断
	UART_IT_TC   = (1 << 1),		//发送完成中断
	UART_IT_TBWE = (1 << 8),		//发送缓冲错误中断
	UART_IT_TBWO = (1 << 9),		//发送缓冲溢出中断
	UART_IT_RB   = (1 << 16),		//接收缓冲器满中断
	UART_IT_ID   = (1 << 17),		//接收空闲帧中断
	UART_IT_RO   = (1 << 24),		//接收数据溢出中断
	UART_IT_FE   = (1 << 25),		//接收帧错误中断
	UART_IT_PE   = (1 << 26),		//接收校验错误中断
	UART_IT_BDE  = (1 << 27),		//波特率检测错误中断
	UART_IT_RBRE = (1 << 28),		//读接收缓冲错误中断
	UART_IT_RBRO = (1 << 29),		//读接收缓冲溢出中断
} UART_TYPE_IT;

/* Interrupt Flag */
typedef enum {
	UART_FLAG_TB   = (1 << 0),		//发送缓冲器空中断
	UART_FLAG_TC   = (1 << 1),		//发送完成中断
	UART_FLAG_TBWE = (1 << 8),		//发送缓冲错误中断
	UART_FLAG_TBWO = (1 << 9),		//发送缓冲溢出中断
	UART_FLAG_RB   = (1 << 16),		//接收缓冲器满中断
	UART_FLAG_ID   = (1 << 17),		//接收空闲帧中断
	UART_FLAG_RO   = (1 << 24),		//接收数据溢出中断
	UART_FLAG_FE   = (1 << 25),		//接收帧错误中断
	UART_FLAG_PE   = (1 << 26),		//接收校验错误中断
	UART_FLAG_BDE  = (1 << 27),		//波特率检测错误中断
	UART_FLAG_RBRE = (1 << 28),		//读接收缓冲错误中断
	UART_FLAG_RBRO = (1 << 29),		//读接收缓冲溢出中断
} UART_TYPE_FLAG;

/* Interrupt Clear Flag */
typedef enum {
	UART_CLR_TC   = (1 << 1),		//发送完成中断
	UART_CLR_TBWE = (1 << 8),		//发送缓冲错误中断
	UART_CLR_TBWO = (1 << 9),		//发送缓冲溢出中断
	UART_CLR_ID   = (1 << 17),		//接收空闲帧中断
	UART_CLR_RO   = (1 << 24),		//接收数据溢出中断
	UART_CLR_FE   = (1 << 25),		//接收帧错误中断
	UART_CLR_PE   = (1 << 26),		//接收校验错误中断
	UART_CLR_BDE  = (1 << 27),		//波特率检测错误中断
	UART_CLR_RBRE = (1 << 28),		//读接收缓冲错误中断
	UART_CLR_RBRO = (1 << 29),		//读接收缓冲溢出中断
} UART_CLR_IF;

/* Interrupt Mode */
typedef enum {
	UART_TBIM_Byte     = 0x0,		//字节中断
	UART_TBIM_HalfWord = 0x1,		//半字中断
	UART_TBIM_Word     = 0x2,		//字中断
	UART_TBIM_Full     = 0x3,		//全中断
} UART_TYPE_TRBIM;

/* Status */
typedef enum {
	UART_STA_TBOV   = (1 << 4),		//发送缓冲器溢出状态位
	UART_STA_TXBUSY = (1 << 5),		//发送状态位
	UART_STA_RBOV   = (1 << 12),	//接收缓冲器溢出状态位
	UART_STA_RXBUSY = (1 << 13),	//接收状态位
	UART_STA_FER0   = (1 << 16),	//当前读取BYTE0帧格式错误位
	UART_STA_PER0   = (1 << 17),	//当前读取BYTE0校验错误位
	UART_STA_FER1   = (1 << 18),	//当前读取BYTE1帧格式错误位
	UART_STA_PER1   = (1 << 19),	//当前读取BYTE1校验错误位
	UART_STA_FER2   = (1 << 20),	//当前读取BYTE2帧格式错误位
	UART_STA_PER2   = (1 << 21),	//当前读取BYTE2校验错误位
	UART_STA_FER3   = (1 << 22),	//当前读取BYTE3帧格式错误位
	UART_STA_PER3   = (1 << 23),	//当前读取BYTE3校验错误位
} UART_TYPE_STA;


#define UART0_TxEnable()	(UART0->CON0.TXEN = 1)
#define UART1_TxEnable()	(UART1->CON0.TXEN = 1)
#define UART0_TxDisable()	(UART0->CON0.TXEN = 0)
#define UART1_TxDisable()	(UART1->CON0.TXEN = 0)
#define UART0_RxEnable()	(UART0->CON0.RXEN = 1)
#define UART1_RxEnable()	(UART1->CON0.RXEN = 1)
#define UART0_RxDisable()	(UART0->CON0.RXEN = 0)
#define UART1_RxDisable()	(UART1->CON0.RXEN = 0)

#define UART0_TxRst()		(UART0->CON0.TRST = 1)
#define UART1_TxRst()		(UART1->CON0.TRST = 1)
#define UART0_RxRst()		(UART0->CON0.RRST = 1)
#define UART1_RxRst()		(UART1->CON0.RRST = 1)


/*  Function */
void UART_Init(UART_TypeDef *UARTx, UART_InitStruType *UART_InitStruct);
void UART_AutoBaudConfig(UART_TypeDef *UARTx,UART_AUTO_BAUD_MODE mode);
void UART_BaudFreeConfig(UART_TypeDef *UARTx,UART_AUTO_BAUD_FREE_MODE mode);
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

#endif
