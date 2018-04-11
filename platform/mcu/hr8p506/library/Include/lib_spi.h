/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_spi.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  SPI模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBSPI_H__
#define __LIBSPI_H__

#include "HR8P506.h"
#include "type.h"

/* 数据发送接收方式 */
typedef enum {
	SPI_RiseSendFallRec = 0x0,	//上升沿发送，下降沿接收
	SPI_FallSendRiseRec = 0x1,	//下降沿发送，上升沿接收
	SPI_RiseRecFallSend = 0x2,	//上升沿接收，下降沿发送
	SPI_FallRecRiseSend = 0x3,	//下降沿接收，上升沿发送
} SPI_TYPE_DFS;

/* 工作模式 */
typedef enum {
	SPI_Mode_Master = 0x0,	//主控模式
	SPI_Mode_Slave  = 0x1,	//从动模式
} SPI_TYPE_MODE;

/* 初始化结构体 */
typedef struct {
	uint32_t SPI_Freq;			//SPI频率
	SPI_TYPE_DFS SPI_Df;		//数据格式
	SPI_TYPE_MODE SPI_Mode;		//通讯模式
	uint8_t SPI_DW;				//发送帧位宽
	TYPE_FUNCEN SPI_DelayRec;	//延迟接收使能
	TYPE_FUNCEN SPI_DelaySend;	//发送间隔使能
	uint8_t SPI_SendDelayPeroid;//发送间隔周期
} SPI_InitStruType;

/* 中断状态标志位 */
typedef enum {
	SPI_Flag_TB    = (1 << 0),	//发送缓冲器空中断标志
	SPI_Flag_RB    = (1 << 1),	//接收缓冲器满中断标志
	SPI_Flag_TE    = (1 << 2),	//发送错误中断标志，仅从动模式
	SPI_Flag_RO    = (1 << 3),	//接收错误溢出中断标志
	SPI_Flag_ID    = (1 << 4),	//空闲状态中断标志
	SPI_Flag_NSSIF = (1 << 5),	//片选变化中断标志
	SPI_Flag_TBWE  = (1 << 6),	//发送数据写错误中断标志
	SPI_Flag_NSS   = (1 << 7),	//片选标志位
	SPI_Flag_TBEF0 = (1 << 8),	//TB0空标志位
	SPI_Flag_TBEF1 = (1 << 9),	//TB1空标志位
	SPI_Flag_TBEF2 = (1 << 10),	//TB2空标志位
	SPI_Flag_TBEF3 = (1 << 11),	//TB3空标志位
	SPI_Flag_RBFF0 = (1 << 12),	//RB0满标志位
	SPI_Flag_RBFF1 = (1 << 13),	//RB1满标志位
	SPI_Flag_RBFF2 = (1 << 14),	//RB2满标志位
	SPI_Flag_RBFF3 = (1 << 15),	//RB3满标志位
	SPI_Flag_IDLE  = (1 << 16),	//空闲标志位
	SPI_Flag_TMS   = (1 << 17),	//帧发送间隔状态标志位
}SPI_TYPE_FLAG;

/* 发送缓冲器空/接收缓冲器满中断类型 */
typedef enum {
	SPI_IType_BYTE     = 0x0,	//字节中断
	SPI_IType_HALFWORD = 0x1,	//半字中断
	SPI_IType_WORD     = 0x2,	//字中断
} SPI_TYPE_TRBIM;

/* 中断使能标志位 */
typedef enum {
	SPI_IT_TB   = (1 << 0),		//发送缓冲器空中断
	SPI_IT_RB   = (1 << 1),		//接收缓冲器满中断
	SPI_IT_TE   = (1 << 2),		//发送数据错误中断
	SPI_IT_RO   = (1 << 3),		//接收错误溢出中断
	SPI_IT_ID   = (1 << 4),		//空闲状态中断
	SPI_IT_NSS  = (1 << 5),		//片选变化中断
	SPI_IT_TBWE = (1 << 6),		//发送数据写错误中断
} SPI_TYPE_IT;

/* 清除中断标志位 */
typedef enum {
	SPI_CLR_TE   = (1 << 2),	//发送错误中断标志，仅从动模式
	SPI_CLR_RO   = (1 << 3),	//接收错误溢出中断标志
	SPI_CLR_ID   = (1 << 4),	//空闲状态中断标志
	SPI_CLR_NSS  = (1 << 5),	//片选变化中断标志
	SPI_CLR_TBWE = (1 << 6),	//发送数据写错误中断标志
} SPI_CLR_IF;

/* SPI状态标志位 */
typedef enum {
	SPI_STA_NSS   = (1 << 7),	//片选标志位
	SPI_STA_TBEF0 = (1 << 8),	//TB0空标志位
	SPI_STA_TBEF1 = (1 << 9),	//TB1空标志位
	SPI_STA_TBEF2 = (1 << 10),	//TB2空标志位
	SPI_STA_TBEF3 = (1 << 11),	//TB3空标志位
	SPI_STA_RBFF0 = (1 << 12),	//RB0满标志位
	SPI_STA_RBFF1 = (1 << 13),	//RB1满标志位
	SPI_STA_RBFF2 = (1 << 14),	//RB2满标志位
	SPI_STA_RBFF3 = (1 << 15),	//RB3满标志位
	SPI_STA_IDLE  = (1 << 16),	//空闲状态标志位
} SPI_TYPE_STA;


/* SPI0 */
#define SPI0_Enable()		(SPI0->CON.EN  = 1)
#define SPI0_Disable()		(SPI0->CON.EN  = 0)
#define SPI0_RecEnable()	(SPI0->CON.REN = 1)
#define SPI0_RecDisable()	(SPI0->CON.REN = 0)
#define SPI0_Rst()		(SPI0->CON.RST = 1)

/* SPI1 */
#define SPI1_Enable()		(SPI1->CON.EN  = 1)
#define SPI1_Disable()		(SPI1->CON.EN  = 0)
#define SPI1_RecEnable()	(SPI1->CON.REN = 1)
#define SPI1_RecDisable()	(SPI1->CON.REN = 0)
#define SPI1_Rst()		(SPI1->CON.RST = 1)


/* SPI function */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitStruType *SPI_InitStruct);
void SPI_ITConfig(SPI_TypeDef *SPIx, SPI_TYPE_IT SPI_IE, TYPE_FUNCEN NewState);
void SPI_DataFormatConfig(SPI_TypeDef *SPIx, SPI_TYPE_DFS Type);
void SPI_SendByte(SPI_TypeDef *SPIx, uint8_t Temp);
void SPI_SendHalfWord(SPI_TypeDef *SPIx, uint16_t Temp);
void SPI_SendWord(SPI_TypeDef *SPIx, uint32_t Temp);
uint8_t SPI_RecByte(SPI_TypeDef *SPIx);
uint16_t SPI_RecHalfWord(SPI_TypeDef *SPIx);
uint32_t SPI_RecWord(SPI_TypeDef *SPIx);
void SPI_TBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type);
void SPI_RBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type);
FlagStatus SPI_GetFlagStatus(SPI_TypeDef *SPIx, SPI_TYPE_FLAG Flag);
ITStatus SPI_GetITStatus(SPI_TypeDef *SPIx, SPI_TYPE_IT Flag);
FlagStatus SPI_GetStatus(SPI_TypeDef *SPIx, SPI_TYPE_STA Flag);
void SPI_ClearITPendingBit(SPI_TypeDef *SPIx, SPI_CLR_IF Flag);
void Clear_TBW(SPI_TypeDef *SPIx);
void Clear_RBR(SPI_TypeDef *SPIx);

#endif	/* end of file */
