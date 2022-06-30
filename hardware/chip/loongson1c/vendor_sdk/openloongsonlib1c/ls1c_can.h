
/*
 * File      : ls1c_can.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:  (Pelican Mode) 
 * Date           Author       Notes
 * 2018-01-06     sundm75       first version
 */

#ifndef __OPENLOONGSON_CAN_H
#define __OPENLOONGSON_CAN_H

#define CAN0               ( (CAN_TypeDef* )LS1C_REG_BASE_CAN0)
#define CAN1               ( (CAN_TypeDef* )LS1C_REG_BASE_CAN1)

#define CAN_InitStatus_Failed              ((unsigned char)0x00) /*!< CAN initialization failed */
#define CAN_InitStatus_Success             ((unsigned char)0x01) /*!< CAN initialization OK */

/* 非应答溢出时间次数 */
#define INAK_TIMEOUT        ((unsigned long)0x00000FFF)

#define CAN_SJW_1tq                 ((unsigned char)0x00)  /*!< 1 time quantum */
#define CAN_SJW_2tq                 ((unsigned char)0x01)  /*!< 2 time quantum */
#define CAN_SJW_3tq                 ((unsigned char)0x02)  /*!< 3 time quantum */
#define CAN_SJW_4tq                 ((unsigned char)0x03)  /*!< 4 time quantum */

#define CAN_BS1_1tq                 ((unsigned char)0x00)  /*!< 1 time quantum */
#define CAN_BS1_2tq                 ((unsigned char)0x01)  /*!< 2 time quantum */
#define CAN_BS1_3tq                 ((unsigned char)0x02)  /*!< 3 time quantum */
#define CAN_BS1_4tq                 ((unsigned char)0x03)  /*!< 4 time quantum */
#define CAN_BS1_5tq                 ((unsigned char)0x04)  /*!< 5 time quantum */
#define CAN_BS1_6tq                 ((unsigned char)0x05)  /*!< 6 time quantum */
#define CAN_BS1_7tq                 ((unsigned char)0x06)  /*!< 7 time quantum */
#define CAN_BS1_8tq                 ((unsigned char)0x07)  /*!< 8 time quantum */
#define CAN_BS1_9tq                 ((unsigned char)0x08)  /*!< 9 time quantum */
#define CAN_BS1_10tq                ((unsigned char)0x09)  /*!< 10 time quantum */
#define CAN_BS1_11tq                ((unsigned char)0x0A)  /*!< 11 time quantum */
#define CAN_BS1_12tq                ((unsigned char)0x0B)  /*!< 12 time quantum */
#define CAN_BS1_13tq                ((unsigned char)0x0C)  /*!< 13 time quantum */
#define CAN_BS1_14tq                ((unsigned char)0x0D)  /*!< 14 time quantum */
#define CAN_BS1_15tq                ((unsigned char)0x0E)  /*!< 15 time quantum */
#define CAN_BS1_16tq                ((unsigned char)0x0F)  /*!< 16 time quantum */

#define CAN_BS2_1tq                 ((unsigned char)0x00)  /*!< 1 time quantum */
#define CAN_BS2_2tq                 ((unsigned char)0x01)  /*!< 2 time quantum */
#define CAN_BS2_3tq                 ((unsigned char)0x02)  /*!< 3 time quantum */
#define CAN_BS2_4tq                 ((unsigned char)0x03)  /*!< 4 time quantum */
#define CAN_BS2_5tq                 ((unsigned char)0x04)  /*!< 5 time quantum */
#define CAN_BS2_6tq                 ((unsigned char)0x05)  /*!< 6 time quantum */
#define CAN_BS2_7tq                 ((unsigned char)0x06)  /*!< 7 time quantum */
#define CAN_BS2_8tq                 ((unsigned char)0x07)  /*!< 8 time quantum */

#define CAN_Id_Standard           0
#define CAN_Id_Extended           1
#define CAN_RTR_DATA               0
#define CAN_RTR_Remote            1

/*!< CAN 控制状态寄存器 */
/************************** CAN_MOD 寄存器位定义*******************************/
#define CAN_Mode_RM                 ((unsigned char)0x01)  /*!< 复位模式 */
#define CAN_Mode_LOM                ((unsigned char)0x02)  /*!< 只听模式 1:只听  0:正常  */
#define CAN_Mode_STM                ((unsigned char)0x04)  /*!< 正常工作模式1:自检测  0:正常  */
#define CAN_Mode_AFM                ((unsigned char)0x08)  /*!< 单/双滤波模式 1:单 0: 双*/
#define CAN_Mode_SM                 ((unsigned char)0x10)  /*!< 睡眠模式1: 睡眠 0: 唤醒 */

/************************** CAN_CMR 寄存器位定义*******************************/
 #define  CAN_CMR_TR                         ((unsigned char)0x01)         /*!< 发送请求 1: 当前信息被发送  0: 空 */
 #define  CAN_CMR_AT                         ((unsigned char)0x02)         /*!< 中止发送 1: 等待发送的信息取消  0: 空缺  */
 #define  CAN_CMR_RRB                        ((unsigned char)0x04)         /*!< 释放接收缓冲器  1:释放  0: 无动作 */
 #define  CAN_CMR_CDO                        ((unsigned char)0x08)         /*!< 清除数据溢出  1:清除  0: 无动作    */
//#define  CAN_CMR_GTS                        ((unsigned char)0x10)         /*!STD模式< 睡眠: 1:进入睡眠  0: 唤醒  */
 #define  CAN_CMR_SRR                        ((unsigned char)0x10)         /*!< 自接收请求  1:  0:   */
 #define  CAN_CMR_EFF                        ((unsigned char)0x80)         /*!< 扩展模式 1:扩展帧 0: 标准帧  */

/************************** CAN_SR 寄存器位定义********************************/
 #define  CAN_SR_BBS                         ((unsigned char)0x01)         /*!< 接收缓存器状态1: 满  0: 空 */
 #define  CAN_SR_DOS                         ((unsigned char)0x02)         /*!< 数据溢出状态 1: 溢出  0: 空缺  */
 #define  CAN_SR_TBS                         ((unsigned char)0x04)         /*!< 发送缓存器状态1: 释放  0: 锁定 */
 #define  CAN_SR_TCS                         ((unsigned char)0x08)         /*!< 发送完毕状态1: 完毕  0: 未完毕    */
 #define  CAN_SR_RS                          ((unsigned char)0x10)         /*!< 接收状态1: 接收  0: 空闲  */
 #define  CAN_SR_TS                          ((unsigned char)0x20)         /*!< 发送状态1:  发送 0:  空闲*/
 #define  CAN_SR_ES                          ((unsigned char)0x40)         /*!< 出错状态1:出错 0: 正常 */
 #define  CAN_SR_BS                          ((unsigned char)0x80)         /*!< 总线状态1: 关闭  0: 开启  */
 
/************************** CAN_IR 中断寄存器位定义****************************/
 #define  CAN_IR_RI                          ((unsigned char)0x01)         /*!< 接收中断 */
 #define  CAN_IR_TI                          ((unsigned char)0x02)         /*!< 发送中断 */
 #define  CAN_IR_EI                          ((unsigned char)0x04)         /*!< 错误中断 */
 #define  CAN_IR_DOI                         ((unsigned char)0x08)         /*!< 数据溢出中断  */
 #define  CAN_IR_WUI                         ((unsigned char)0x10)         /*!< 唤醒中断 */
 #define  CAN_IR_EPI                         ((unsigned char)0x20)         /*!< 错误消极中断 */
 #define  CAN_IR_ALI                         ((unsigned char)0x40)         /*!< 仲裁丢失中断 */
 #define  CAN_IR_BEI                         ((unsigned char)0x80)         /*!< 总线错误中断  */
 
/************************* CAN_IER 中断使能寄存器位定义************************/
 #define  CAN_IER_RIE                         ((unsigned char)0x01)        /*!< 接收中断使能 */
 #define  CAN_IER_TIE                         ((unsigned char)0x02)        /*!< 发送中断使能 */
 #define  CAN_IER_EIE                         ((unsigned char)0x04)        /*!< 错误中断使能 */
 #define  CAN_IER_DOIE                        ((unsigned char)0x08)        /*!< 数据溢出中断使能  */
 #define  CAN_IER_WUIE                        ((unsigned char)0x10)        /*!< 唤醒中断使能 */
 #define  CAN_IER_EPIE                        ((unsigned char)0x20)        /*!< 错误消极中断使能 */
 #define  CAN_IER_ALIE                        ((unsigned char)0x40)        /*!< 仲裁丢失中断使能 */
 #define  CAN_IER_BEIE                        ((unsigned char)0x80)        /*!< 总线错误中断使能  */
 
typedef enum 
{
	CAN1MBaud=0,    // 1 MBit/sec
	CAN800kBaud,    // 800 kBit/sec
	CAN500kBaud,    // 500 kBit/sec
	CAN250kBaud,    // 250 kBit/sec
	CAN125kBaud,    // 125 kBit/sec
	CAN100kBaud,    // 100 kBit/sec
	CAN50kBaud,     // 50 kBit/sec
	CAN40kBaud,     // 40 kBit/sec
}Ls1c_CanBPS_t;

typedef struct
{
   unsigned char MOD;
   unsigned char CMR;
   unsigned char SR;
   unsigned char IR;
   unsigned char IER;
   unsigned char reserved0;
   unsigned char BTR0;
   unsigned char BTR1;
   unsigned char OCR;
   unsigned char reserved[2];
   unsigned char ALC;
   unsigned char ECC ;
   unsigned char EMLR;
   unsigned char RXERR;
   unsigned char TXERR;
   unsigned char IDE_RTR_DLC;
   unsigned char ID[4];
   unsigned char BUF[8];
   unsigned char RMCR;
   unsigned char CDR;
} CAN_TypeDef;

typedef struct
{
  unsigned char  CAN_Prescaler;    /* 波特率分频系数1 to 31. */  
  unsigned char  CAN_Mode;         /*0x10:睡眠0x08:单,双滤波 0x40:正常工作0x20:只听 0x01:复位*/
  unsigned char  CAN_SJW;          /*同步跳转宽度 */
  unsigned char  CAN_BS1;          /*时间段1计数值*/
  unsigned char  CAN_BS2;          /*时间段2计数值*/ 
  
} CAN_InitTypeDef;

typedef struct
{
  unsigned char  CAN_FilterId0;        /*验收代码0*/
  unsigned char  CAN_FilterMaskId0;    /*验收屏蔽0*/
  unsigned char  CAN_FilterId1;        /*验收代码1*/
  unsigned char  CAN_FilterMaskId1;    /*验收屏蔽1*/
  unsigned char  CAN_FilterId2;        /*验收代码2*/
  unsigned char  CAN_FilterMaskId2;    /*验收屏蔽2*/
  unsigned char  CAN_FilterId3;        /*验收代码3*/
  unsigned char  CAN_FilterMaskId3;    /*验收屏蔽3*/
} CAN_FilterInitTypeDef;

typedef struct
{
  unsigned long StdId;  /* 11位ID*/
  unsigned long ExtId;  /*29位ID**/
  unsigned char IDE;    /*IDE: 标识符选择
                                     该位决定发送邮箱中报文使用的标识符类型
                                     0: 使用标准标识符
                                     1: 使用扩展标识符*/
  unsigned char RTR;     /*远程发送请求
                                       0: 数据帧
                                       1: 远程帧*/
  unsigned char DLC;     /*数据帧长度*/
  unsigned char Data[8]; /*8字节数据*/
} CanRxMsg;

typedef struct
{
  unsigned long StdId;  /* 11位ID*/
  unsigned long ExtId;  /*29位ID**/
  unsigned char IDE;    /*IDE: 标识符选择
                                     该位决定发送邮箱中报文使用的标识符类型
                                     0: 使用标准标识符
                                     1: 使用扩展标识符*/
  unsigned char RTR;     /*远程发送请求
                                       0: 数据帧
                                       1: 远程帧*/
  unsigned char DLC;     /*数据帧长度*/
  unsigned char Data[8]; /*8字节数据*/
} CanTxMsg;

unsigned char CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct);
void CAN_FilterInit(CAN_TypeDef* CANx,  CAN_FilterInitTypeDef* CAN_FilterInitStruct);
unsigned char CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage);
void CAN_Receive(CAN_TypeDef* CANx,  CanRxMsg* RxMessage);

#endif

