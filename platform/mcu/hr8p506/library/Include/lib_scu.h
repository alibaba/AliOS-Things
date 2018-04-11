/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_scu.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  SCU模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBSCU_H__
#define __LIBSCU_H__

#include "HR8P506.h"
#include "lib_gpio.h"
#include "type.h"

/* NMI不可屏蔽中断选择 */
typedef enum
{
	SCU_NMIIRQ_PINT0    = 0, 
	SCU_NMIIRQ_PINT1    = 1, 
	SCU_NMIIRQ_PINT2    = 2, 
	SCU_NMIIRQ_PINT3    = 3, 
	SCU_NMIIRQ_PINT4    = 4, 
	SCU_NMIIRQ_PINT5    = 5, 
	SCU_NMIIRQ_PINT6    = 6, 
	SCU_NMIIRQ_PINT7    = 7, 
	SCU_NMIIRQ_T16N0    = 8, 
	SCU_NMIIRQ_T16N1    = 9, 
	SCU_NMIIRQ_T16N2    = 10, 
	SCU_NMIIRQ_T16N3    = 11, 
	SCU_NMIIRQ_T32N0    = 12, 
	SCU_NMIIRQ_WDTINT   = 16, 
	SCU_NMIIRQ_RTCINT   = 17, 
	SCU_NMIIRQ_KINT     = 18, 
	SCU_NMIIRQ_ADCINT   = 19, 
	SCU_NMIIRQ_LVDINT   = 21, 
	SCU_NMIIRQ_PLLLKINT = 22,
	SCU_NMIIRQ_UART0    = 23, 
	SCU_NMIIRQ_UART1    = 24, 
	SCU_NMIIRQ_EUART0   = 25, 
	SCU_NMIIRQ_SPI0INT  = 27, 
	SCU_NMIIRQ_SPI1INT  = 28, 
	SCU_NMIIRQ_IIC0INT  = 29, 
	SCU_NMIIRQ_CCMINT   = 31, 
} SCU_TYPE_NMICS;

/* PWRC复位状态寄存器标志位 */
typedef enum
{
	SCU_PWRC_PORF     = (1 << 0),		//POR总复位标志
	SCU_PWRC_PORRCF   = (1 << 1),    	//PORRC复位标志
	SCU_PWRC_PORRSTF  = (1 << 2),  		//PORRST复位标志
	SCU_PWRC_BORF     = (1 << 3),    	//BOR复位标志
	SCU_PWRC_WDTRSTF  = (1 << 4), 		//WDT复位标志
	SCU_PWRC_MRSTF    = (1 << 5),  		//MRSTn丢失标志位
	SCU_PWRC_SOFTRSTF = (1 << 6),  		//软件丢失标志位
	SCU_PWRC_POR_LOST = (1 << 7),  		//POR丢失标志位	
	SCU_PWRC_CFGRST   = (1 << 8),  		//配置字读取
} SCU_TYPE_PWRC;

/* LVD寄存器标志位 */
typedef enum
{
	SCU_LVDFlag_IF  = (1 << 8),  //LVD中断标志
	SCU_LVDFlag_Out = (1 << 15), //输出状态位
} SCU_TYPE_LVDCON;

/* 时钟选择 */
typedef enum
{
	SCU_CLK_HRC  = 0x0 ,  	//16MHz RC时钟
	SCU_CLK_LRC  = 0x1 ,  	//32KHZ RC时钟
	SCU_CLK_XTAL = 0x2 ,   	//XTAL时钟
} SCU_TYPE_SYSCLK;

/* 外设时钟 */
typedef enum
{
	SCU_SUCCLK    = 0x00000001,
	SCU_GPIOCLK   = 0x00000002,
	SCU_IAPCLK    = 0x00000004, 
	SCU_ADCCLK    = 0x00000010,
	SCU_RTCCLK    = 0x00000020,
	SCU_LCDCLK    = 0x00000040,
	SCU_WDTCLK    = 0x00000080,
	SCU_T16N0CLK  = 0x00000100,
	SCU_T16N1CLK  = 0x00000200,
	SCU_T16N2CLK  = 0x00000400,
	SCU_T16N3CLK  = 0x00000800,
	SCU_T32N0CLK  = 0x00001000,
	SCU_UART0CLK  = 0x00010000,
	SCU_UART1CLK  = 0x00020000,
	SCU_EUART0CLK = 0x00100000,
	SCU_SPIOCLK   = 0x01000000,
	SCU_SPI1CLK   = 0x02000000,
	SCU_I2C0CLK   = 0x10000000,

} SUC_TYPE_Periph;


/* PLL时钟源选择 */
typedef enum
{
	SCU_PLL_IN4M   = 0x0 ,  	//PLL时钟源为内部4MHz
	SCU_PLL_IN16M  = 0x1 ,  	//PLL时钟源为内部16MHz
	SCU_PLL_IN32K  = 0x2 ,   	//PLL时钟源为内部32KHz
	SCU_PLL_OUT4M  = 0x3 ,    //PLL时钟源为外部4MHz  需配置CFG_OSCMD为HS
	SCU_PLL_OUT8M  = 0x4,     //PLL时钟源为外部8MHz  需配置CFG_OSCMD为HS
	SCU_PLL_OUT16M = 0x5,     //PLL时钟源为外部16MHz 需配置CFG_OSCMD为HS
	SCU_PLL_OUT32K = 0x6,     //PLL时钟源为外部32KHz 需配置CFG_OSCMD为LP
} SCU_PLL_Origin;

/* PLL时钟输出频率旋择 */
typedef enum
{
	SCU_PLL_32M   = 0x0 ,  	//PLL时钟输出为32MHz
	SCU_PLL_48M   = 0x1 ,  	//PLL时钟输出为48Mhz

} SCU_PLL_Out;





/************SCU模块宏定义***********/

/* SCU写保护控制 */
#define SCU_RegUnLock()	(SCU->PROT.Word = 0x55AA6996)
#define SCU_RegLock() 	(SCU->PROT.Word = 0x00000000)

/* NMI使能控制 */
#define SCU_NMI_Enable()  	(SCU->NMICON.NMIEN = 0x1)
#define SCU_NMI_Disable()	  (SCU->NMICON.NMIEN = 0x0)

/*-------LVD模块-------*/

/* LVD使能控制 */
#define SCU_LVD_Enable()  	(SCU->LVDCON.EN = 0x1)
#define SCU_LVD_Disable() 	(SCU->LVDCON.EN = 0x0)

/* LVD滤波使能控制 */
#define SCU_LVDFLT_Enable()		(SCU->LVDCON.FLTEN = 0x1) 
#define SCU_LVDFLT_Disable()	(SCU->LVDCON.FLTEN = 0x0) 

/* LVD触发电压选择 */
#define SCU_LVDVS_2V() 	  (SCU->LVDCON.VS = 0x0)
#define SCU_LVDVS_2V1() 	(SCU->LVDCON.VS = 0x1)
#define SCU_LVDVS_2V2() 	(SCU->LVDCON.VS = 0x2)
#define SCU_LVDVS_2V4()  	(SCU->LVDCON.VS = 0x3)
#define SCU_LVDVS_2V6() 	(SCU->LVDCON.VS = 0x4)
#define SCU_LVDVS_2V8() 	(SCU->LVDCON.VS = 0x5)
#define SCU_LVDVS_3V() 	  (SCU->LVDCON.VS = 0x6)
#define SCU_LVDVS_3V6() 	(SCU->LVDCON.VS = 0x7)
#define SCU_LVDVS_4V()  	(SCU->LVDCON.VS = 0x8)
#define SCU_LVDVS_4V6() 	(SCU->LVDCON.VS = 0x9)
#define SCU_LVDVS_2V3() 	(SCU->LVDCON.VS = 0xA)
#define SCU_LVDVS_LVDIN() 	(SCU->LVDCON.VS = 0xF)

/* LVD中断使能控制 */
#define SCU_LVDIT_Enable()  (SCU->LVDCON.IE = 0x1)
#define SCU_LVDIT_Disable() (SCU->LVDCON.IE = 0x0)

/* LVD中断标志位清除 */
#define SCU_LVDClearIFBit()	(SCU->LVDCON.IF = 1)

/* LVD中断产生模式选择 */
#define SCU_LVDIFS_Rise() 	(SCU->LVDCON.IFS = 0x0) //LVDO上升沿产生中断
#define SCU_LVDIFS_Fall() 	(SCU->LVDCON.IFS = 0x1) //LVDO下降沿产生中断
#define SCU_LVDIFS_High() 	(SCU->LVDCON.IFS = 0x2) //LVDO高电平产生中断
#define SCU_LVDIFS_Low()  	(SCU->LVDCON.IFS = 0x3) //LVDO低电平产生中断 
#define SCU_LVDIFS_Change() (SCU->LVDCON.IFS = 0x4) //LVDO电平变化产生中断

/* FLASH访问等待时间选择 */
#define SCU_FlashWait_1Tclk()	(SCU->FLASHWAIT.ACCT = 0x0)
#define SCU_FlashWait_2Tclk() 	(SCU->FLASHWAIT.ACCT = 0x1)
#define SCU_FlashWait_3Tclk() 	(SCU->FLASHWAIT.ACCT = 0x2)
#define SCU_FlashWait_4Tclk() 	(SCU->FLASHWAIT.ACCT = 0x3)
#define SCU_FlashWait_5Tclk() 	(SCU->FLASHWAIT.ACCT = 0x4)
#define SCU_FlashWait_6Tclk() 	(SCU->FLASHWAIT.ACCT = 0x5)
#define SCU_FlashWait_7Tclk() 	(SCU->FLASHWAIT.ACCT = 0x6)
#define SCU_FlashWait_8Tclk() 	(SCU->FLASHWAIT.ACCT = 0x7)
#define SCU_FlashWait_9Tclk() 	(SCU->FLASHWAIT.ACCT = 0x8)
#define SCU_FlashWait_10Tclk() 	(SCU->FLASHWAIT.ACCT = 0x9)
#define SCU_FlashWait_11Tclk() 	(SCU->FLASHWAIT.ACCT = 0xA)
#define SCU_FlashWait_12Tclk() 	(SCU->FLASHWAIT.ACCT = 0xB)
#define SCU_FlashWait_13Tclk() 	(SCU->FLASHWAIT.ACCT = 0xC)
#define SCU_FlashWait_14Tclk() 	(SCU->FLASHWAIT.ACCT = 0xD)
#define SCU_FlashWait_15Tclk() 	(SCU->FLASHWAIT.ACCT = 0xE)
#define SCU_FlashWait_16Tclk() 	(SCU->FLASHWAIT.ACCT = 0xF)

/* 系统时钟后分频选择 */
#define SCU_SysClk_Div1() 	(SCU->SCLKEN0.SYSCLKDIV = 0)
#define SCU_SysClk_Div2() 	(SCU->SCLKEN0.SYSCLKDIV = 1)
#define SCU_SysClk_Div4() 	(SCU->SCLKEN0.SYSCLKDIV = 2)
#define SCU_SysClk_Div8() 	(SCU->SCLKEN0.SYSCLKDIV = 3)
#define SCU_SysClk_Div16() 	(SCU->SCLKEN0.SYSCLKDIV = 4)
#define SCU_SysClk_Div32() 	(SCU->SCLKEN0.SYSCLKDIV = 5)
#define SCU_SysClk_Div64() 	(SCU->SCLKEN0.SYSCLKDIV = 6)
#define SCU_SysClk_Div128() (SCU->SCLKEN0.SYSCLKDIV = 7)

/* 外部时钟低功耗模式 */
#define SCU_XTAL_LP_Enable()  	(SCU->SCLKEN0.XTAL_LP = 0)
#define SCU_XTAL_LP_Disable()	(SCU->SCLKEN0.XTAL_LP = 1)



/* 中断向量表重映射使能控制 */
#define SCU_TBLRemap_Enable()  	(SCU->TBLREMAPEN.EN= 1)
#define SCU_TBLRemap_Disable() 	(SCU->TBLREMAPEN.EN= 0)

/* 中断向量表偏移寄存器 x最大为2^24=16777216 */
#define SCU_TBL_Offset(x) 	(SCU->TBLOFF.TBLOFF = (uint32_t)x)

/************SCU模块函数声明***********/
void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type);
FlagStatus SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag);
void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag);
void SCU_OpenXTAL(void);
FlagStatus SCU_GetLVDFlagStatus(SCU_TYPE_LVDCON LVD_Flag);
void SCU_SysClkSelect(SCU_TYPE_SYSCLK Sysclk);
SCU_TYPE_SYSCLK SCU_GetSysClk(void);
void DeviceClockAllEnable(void);
void DeviceClockAllDisable(void);
void DeviceClock_Config(SUC_TYPE_Periph tppe_periph ,TYPE_FUNCEN NewState );
void PLLClock_Config(TYPE_FUNCEN pll_en , SCU_PLL_Origin  pll_origin ,SCU_PLL_Out pll_out,TYPE_FUNCEN sys_pll );
void SCU_MultTimerEnable(SCU_TIMEREN_Typedef SCU_TimerMask);
void SCU_MultTimerDisable(SCU_TIMERDIS_Typedef SCU_TimerMask);

#endif
