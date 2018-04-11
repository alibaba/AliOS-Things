/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_euart.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  EUART模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBEUART_H__
#define __LIBEUART_H__

#include "HR8P506.h"
#include "lib_uart.h"
#include "type.h"

/* 停止位 */
typedef enum {
	EUART_StopBits_1 = 0x0,			// 1位停止位
	EUART_StopBits_2 = 0x1,			// 2位停止位
} EUART_TYPE_TXFS;

/* 数据格式 */
typedef enum {
	EUART_DataMode_8     = 0x0,		//8位数据
	EUART_DataMode_9     = 0x2,		//9位数据
	EUART_DataMode_7     = 0x4,		//7位数据
	EUART_DataMode_8Even = 0x8,		//8位数据+偶校验
	EUART_DataMode_8Odd  = 0x9,		//8位数据+奇校验
	EUART_DataMode_8Add0 = 0xa,		//8位数据+固定0
	EUART_DataMode_8Add1 = 0xb,		//8位数据+固定1
	EUART_DataMode_7Even = 0xc,		//7位数据+偶校验
	EUART_DataMode_7Odd  = 0xd,		//7位数据+奇校验
	EUART_DataMode_7Add0 = 0xe,		//7位数据+固定0
	EUART_DataMode_7Add1 = 0xf,		//7位数据+固定1
} EUART_TYPE_DATAMOD;

/* 端口极性选择 */
typedef enum {
	EUART_Polar_Normal   = 0x0,		//正极性
	EUART_Polar_Opposite = 0x01,	//负极性
} EUART_TYPE_RTXP;

/* 时钟选择 */
typedef enum {
	EUART_Clock_1 = 0x1,	// PCLK / 1
	EUART_Clock_2 = 0x2,	// PCLK / 2
	EUART_Clock_3 = 0x3,	// PCLK / 4
	EUART_Clock_4 = 0x4,	// PCLK / 8
} EUART_TYPE_BCS;

/* 初始化结构体 */
typedef struct {
	EUART_TYPE_TXFS    EUART_StopBits;	//停止位选择
	EUART_TYPE_DATAMOD EUART_TxMode;	//发送数据格式
	EUART_TYPE_RTXP    EUART_TxPolar;	//发送端口极性
	EUART_TYPE_DATAMOD EUART_RxMode;	//接收数据格式
	EUART_TYPE_RTXP    EUART_RxPolar;	//接收端口极性
	uint32_t           EUART_BaudRate;	//波特率
	EUART_TYPE_BCS     EUART_ClockSet;	//EUART时钟选择
} EUART_InitStruType;

/* 中断使能标志位 */
typedef enum {
	EUART_IT_TB   = (1 << 0),	//发送缓冲器空中断
	EUART_IT_TC   = (1 << 1),	//发送完成中断
	EUART_IT_TBWE = (1 << 8),	//写发送缓冲错误中断
	EUART_IT_ARTE = (1 << 12),	//自动重发失败中断
	EUART_IT_RNA  = (1 << 13),	//接收到错误信号中断
	EUART_IT_RB   = (1 << 16),	//接收缓冲器满中断
	EUART_IT_RO   = (1 << 24),	//接收缓冲器溢出中断
	EUART_IT_FE   = (1 << 25),	//接收帧错误中断
	EUART_IT_PE   = (1 << 26),	//接收校验错误中断
	EUART_IT_RBRE = (1 << 28),	//读接收缓冲错误中断
} EUART_TYPE_IT;

/* 中断状态标志位 */
typedef enum {
	EUART_FLAG_TB   = (1 << 0),		//发送缓冲器空中断标志
	EUART_FLAG_TC   = (1 << 1),		//发送完成中断标志
	EUART_FLAG_TBWE = (1 << 8),		//写发送缓冲错误中断标志
	EUART_FLAG_ARTE = (1 << 12),	//自动重发失败中断标志
	EUART_FLAG_RNA  = (1 << 13),	//接收到错误信号中断标志
	EUART_FLAG_RB   = (1 << 16),	//接收缓冲器满中断标志
	EUART_FLAG_RO   = (1 << 24),	//接收缓冲器溢出中断标志
	EUART_FLAG_FE   = (1 << 25),	//接收帧中断标志
	EUART_FLAG_PE   = (1 << 26),	//接收校验中断标志
	EUART_FLAG_RBRE = (1 << 28),	//读接收缓冲错误中断标志
} EUART_TYPE_FLAG;

/* 清除中断状态标志位 */
typedef enum {
	EUART_CLR_TC   = (1 << 1),		//发送完成中断标志
	EUART_CLR_TBWE = (1 << 8),		//写发送缓冲错误中断标志
	EUART_CLR_ARTE = (1 << 12),		//自动重发失败中断标志
	EUART_CLR_RNA  = (1 << 13),		//接收到错误信号中断标志
	EUART_CLR_RO   = (1 << 24),		//接收缓冲器溢出中断标志
	EUART_CLR_FE   = (1 << 25),		//接收帧中断标志
	EUART_CLR_PE   = (1 << 26),		//接收校验中断标志
	EUART_CLR_RBRE = (1 << 28),		//读接收缓冲错误中断标志
} EUART_CLR_IF;

/* 发送缓冲器空/接收缓冲器满中断类型 */
typedef enum {
	EUART_TRBIM_Byte     = 0x0,		//字节空中断
	EUART_TRBIM_HalfWord = 0x1,		//半字空中断
	EUART_TRBIM_Word     = 0x2,		//字空中断
} EUART_TYPE_TRBIM;

/* U7816 Module */
/* EUART模式选择 */
typedef enum {
	EUART_Mode_Uart  = 0x0,		//通用UART模式
	EUART_Mode_U7816 = 0x1,		//7816通讯模式
} EUART_TYPE_MODE;

/* EIO端口选择 */
typedef enum {
	U7816_CHS_EIO0 = 0x0,		//EIO端口0
	U7816_CHS_EIO1 = 0x1,		//EIO端口1
} U7816_TYPE_CHS;

/* EIO方向选择 */
typedef enum {
	U7816_EIODir_In  = 0x0,		//接收数据
	U7816_EIODir_Out = 0x1,		//发送数据
} U7816_TYPE_IOC;

/* 数据格式 */
typedef enum {
	U7816_DataForm_Normal   = 0x0,	//正向
	U7816_DataForm_Contrary = 0x1,	//反向
} U7816_TYPE_DAS;

/* 校验选择 */
typedef enum {
	U7816_Verify_Odd  = 0x0,	//奇校验
	U7816_Verify_Even = 0x1,	//偶校验
} U7816_TYPE_PS;

/* NACK宽度 */
typedef enum {
	U7816_NACKWidth_1ETU   = 0x0,	//1个ETU
	U7816_NACKWidth_1P5ETU = 0x1,	//1.5个ETU
	U7816_NACKWidth_2ETU   = 0x2,	//2个ETU
} U7816_TYPE_TNAS;

/* 重发次数 */
typedef enum {
	U7816_RetryTime_1 = 0x0,	//重发1次
	U7816_RetryTime_2 = 0x1,	//重发2次
	U7816_RetryTime_3 = 0x2,	//重发3次
	U7816_RetryTime_N = 0x3,	//重发无限次，直到发送成功
} U7816_TYPE_ARTS;

/* 时钟选择 */
typedef enum {
	U7816_PCLK_1 = 0x0,		//PCLK
	U7816_PCLK_2 = 0x1,		//PCLK / 2
	U7816_PCLK_4 = 0x2,		//PCLK / 4
	U7816_PCLK_8 = 0x3,		//PCLK / 8
} U7816_TYPE_CKS;

/* 7816初始化结构体 */
typedef struct {
	EUART_TYPE_BCS  U7816_ClockSet;		//时钟选择
	uint32_t        U7816_BaudRate;		//波特率
	TYPE_FUNCEN     U7816_ECK0;			//ECK0使能
	TYPE_FUNCEN     U7816_ECK1;			//ECK1使能
	U7816_TYPE_CHS  U7816_EIOCh;		//EIO通讯通道选择位
	U7816_TYPE_IOC  U7816_EIODir;		//EIO端口方向
	U7816_TYPE_DAS  U7816_DataForm;		//数据格式
	U7816_TYPE_PS   U7816_DataVerify;	//奇偶校验
	TYPE_FUNCEN     U7816_AutoRetryTx;	//自动重发
	TYPE_FUNCEN     U7816_AutoRetryRx;	//自动重收
	U7816_TYPE_TNAS U7816_NACK_Width;	//NACK信号宽度
	U7816_TYPE_ARTS U7816_RetryTimes;	//自动次数
	U7816_TYPE_CKS  U7816_CLKS;			//时钟源选择
	uint8_t         U7816_ETUTime;		//ETU保护时间
} U7816_InitStruType;


#define EUART_TxEnable()	(EUART0->CON0.TXEN = 1)
#define EUART_TxDisable()	(EUART0->CON0.TXEN = 0)
#define EUART_RxEnable()	(EUART0->CON0.RXEN = 1)
#define EUART_RxDisable()	(EUART0->CON0.RXEN = 0)

#define EUART_TxRst()		(EUART0->CON0.TRST = 1)
#define EUART_RxRst()		(EUART0->CON0.RRST = 1)
#define EUART_U7816_REST()	(EUART0->CON2.ERST = 1)

void EUART_ModeConfig(EUART_TYPE_MODE Mode);
void EUART_Init(EUART_InitStruType *EUART_InitStruct);
void EUART_BaudConfig(uint32_t BaudRate, EUART_TYPE_BCS ClockSet);
void EUART_ITConfig(EUART_TYPE_IT EUART_IT, TYPE_FUNCEN NewState);
void EUART_TBIMConfig(EUART_TYPE_TRBIM Type);
void EUART_RBIMConfig(EUART_TYPE_TRBIM Type);
FlagStatus EUART_GetFlagStatus(EUART_TYPE_FLAG EUART_Flag);
ITStatus EUART_GetITStatus(EUART_TYPE_IT EUART_Flag);
void EUART_ClearITPendingBit(EUART_CLR_IF EUART_Flag);

void U7816_Init(U7816_InitStruType *U7816_InitStruct);
void EUART_EIOChConfig(U7816_TYPE_CHS U7816_IO);
void EUART_EIODirection(U7816_TYPE_IOC Dir);
void EUART_SendByte(uint8_t ByteData);
void EUART_SendHalfWord(uint16_t HalfWordData);
void EUART_SendWord(uint32_t WordData);
uint8_t EUART_RecByte(void);
uint16_t EUART_RecHalfWord(void);
uint32_t EUART_RecWord(void);

#endif
