/**
  ******************************************************************************
  * @file    fm33a0xx_rtc.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    15-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of RTC peripheral:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_rtc.h"  

/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup RTC 
  * @brief RCC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
//Code_Start
//2018-04-02-13-22-12
//FM33A0XX_Driver_Gen_V1.4

/************************************************************************************
RTC写使能寄存器 相关函数 
功能:是否允许CPU向RTC的BCD时间寄存器写入初值
输入:写入0xACACACAC  , 允许CPU向RTC的BCD时间寄存器写入初值, RTCWE置1
	     写入不为0xACACACAC任意值,   恢复写保护,RTCWE清0

**************************************************************************************/
void RTC_RTCWE_Write(uint32_t SetValue)
{
	RTC->RTCWE = (SetValue);
}

/********************************
BCD时间秒时间写入函数
功能:写入秒时间数值
输入:秒时间数值,BCD格式

********************************/
void RTC_BCDSEC_Write(uint32_t SetValue)
{
	RTC->BCDSEC = (SetValue & RTC_BCDSEC_BCDSEC_Msk);
}
/********************************
BCD时间秒时间读取函数
功能:读取秒时间数值
输出:秒时间输出，格式BCD

********************************/
uint32_t RTC_BCDSEC_Read(void)
{
	return (RTC->BCDSEC & RTC_BCDSEC_BCDSEC_Msk);
}
/********************************
BCD时间分钟时间写入函数
功能:写入分钟时间数值
输入:分钟时间数值,BCD格式

********************************/
void RTC_BCDMIN_Write(uint32_t SetValue)
{
	RTC->BCDMIN = (SetValue & RTC_BCDMIN_BCDMIN_Msk);
}
/********************************
BCD时间分钟时间读取函数
功能:读取分钟时间数值
输出:分钟时间输出，格式BCD

********************************/
uint32_t RTC_BCDMIN_Read(void)
{
	return (RTC->BCDMIN & RTC_BCDMIN_BCDMIN_Msk);
}

/********************************
BCD时间小时时间写入函数
功能:写入小时时间数值
输入:小时时间数值,BCD格式

********************************/
void RTC_BCDHOUR_Write(uint32_t SetValue)
{
	RTC->BCDHOUR = (SetValue & RTC_BCDHOUR_BCDHOUR_Msk);
}
/********************************
BCD时间小时时间读取函数
功能:读取小时时间数值
输出:小时时间输出，格式BCD

********************************/
uint32_t RTC_BCDHOUR_Read(void)
{
	return (RTC->BCDHOUR & RTC_BCDHOUR_BCDHOUR_Msk);
}

/* BCD时间天寄存器 相关函数 */
void RTC_BCDDATE_Write(uint32_t SetValue)
{
	RTC->BCDDATE = (SetValue & RTC_BCDDATE_BCDDATE_Msk);
}

uint32_t RTC_BCDDATE_Read(void)
{
	return (RTC->BCDDATE & RTC_BCDDATE_BCDDATE_Msk);
}

/* BCD时间星期寄存器 相关函数 */
void RTC_BCDWEEK_Write(uint32_t SetValue)
{
	RTC->BCDWEEK = (SetValue & RTC_BCDWEEK_BCDWEEK_Msk);
}

uint32_t RTC_BCDWEEK_Read(void)
{
	return (RTC->BCDWEEK & RTC_BCDWEEK_BCDWEEK_Msk);
}

/********************************
BCD时间月时间写入函数
功能:写入月时间数值
输入:月时间数值,BCD格式

********************************/
void RTC_BCDMONTH_Write(uint32_t SetValue)
{
	RTC->BCDMONTH = (SetValue & RTC_BCDMONTH_BCDMONTH_Msk);
}
/********************************
BCD时间月时间读取函数
功能:读取月时间数值
输出:月时间输出，格式BCD

********************************/
uint32_t RTC_BCDMONTH_Read(void)
{
	return (RTC->BCDMONTH & RTC_BCDMONTH_BCDMONTH_Msk);
}


/********************************
BCD时间年时间写入函数
功能:写入年时间数值
输入:年时间数值,BCD格式

********************************/
void RTC_BCDYEAR_Write(uint32_t SetValue)
{
	RTC->BCDYEAR = (SetValue & RTC_BCDYEAR_BCDYEAR_Msk);
}
/********************************
BCD时间年时间读取函数
功能:读取年时间数值
输出:年时间输出，格式BCD

********************************/
uint32_t RTC_BCDYEAR_Read(void)
{
	return (RTC->BCDYEAR & RTC_BCDYEAR_BCDYEAR_Msk);
}


/********************************
闹钟小时设置函数
功能:设置闹钟小时数值
输入:设置闹钟小时数据

********************************/
void RTC_ALARM_ALARMHOUR_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = RTC->ALARM;
	tmpreg &= ~(RTC_ALARM_ALARMHOUR_Msk);
	tmpreg |= (SetValue & RTC_ALARM_ALARMHOUR_Msk);
	RTC->ALARM = tmpreg;
}
/********************************
闹钟小时读取函数
功能:读取闹钟小时数值
输出:读取闹钟小时数据

********************************/
uint32_t RTC_ALARM_ALARMHOUR_Get(void)
{
	return (RTC->ALARM & RTC_ALARM_ALARMHOUR_Msk);
}

/********************************
闹钟分钟设置函数
功能:设置闹钟分钟数值
输入:设置闹钟分钟数据

********************************/
void RTC_ALARM_ALARMMIN_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = RTC->ALARM;
	tmpreg &= ~(RTC_ALARM_ALARMMIN_Msk);
	tmpreg |= (SetValue & RTC_ALARM_ALARMMIN_Msk);
	RTC->ALARM = tmpreg;
}
/********************************
闹钟分钟读取函数
功能:读取分钟数值
输出:读取分钟数据

********************************/
uint32_t RTC_ALARM_ALARMMIN_Get(void)
{
	return (RTC->ALARM & RTC_ALARM_ALARMMIN_Msk);
}

/********************************
闹钟秒设置函数
功能:设置闹钟秒数值
输入:设置闹钟秒数据

********************************/
void RTC_ALARM_ALARMSEC_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = RTC->ALARM;
	tmpreg &= ~(RTC_ALARM_ALARMSEC_Msk);
	tmpreg |= (SetValue & RTC_ALARM_ALARMSEC_Msk);
	RTC->ALARM = tmpreg;
}
/********************************
闹钟秒读取函数
功能:读取秒数值
输出:读取秒数据

********************************/
uint32_t RTC_ALARM_ALARMSEC_Get(void)
{
	return (RTC->ALARM & RTC_ALARM_ALARMSEC_Msk);
}

/********************************
频率信号输出选择函数
功能:频率信号输出选择
输出:信号选择代号

********************************/
void RTC_FSEL_FSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = RTC->FSEL;
	tmpreg &= ~(RTC_FSEL_FSEL_Msk);
	tmpreg |= (SetValue & RTC_FSEL_FSEL_Msk);
	RTC->FSEL = tmpreg;
}
/********************************
输出信号的代号获取函数
功能:输出信号的代号获取
输出:信号选择代号

********************************/
uint32_t RTC_FSEL_FSEL_Get(void)
{
	return (RTC->FSEL & RTC_FSEL_FSEL_Msk);
}

/********************************
时基计数器数值调整函数
功能:时基计数器数值调整，调整时基输出的准确性
输入:时基计数器数值

********************************/
void RTC_ADJUST_Write(uint32_t SetValue)
{
	RTC->ADJUST = (SetValue & RTC_ADJUST_ADJUST_Msk);
}
/********************************
时基计数器数值读取函数
功能:时基计数器数值的读取
输出:时基计数器数值

********************************/
uint32_t RTC_ADJUST_Read(void)
{
	return (RTC->ADJUST & RTC_ADJUST_ADJUST_Msk);
}

/********************************
时基计数器数值调整方向函数
功能:时基计数器数值调整的方向调整
输入:	0：表示增加计数初值
	1：表示减少计数初值


********************************/
void RTC_ADSIGN_ADSIGN_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = RTC->ADSIGN;
	tmpreg &= ~(RTC_ADSIGN_ADSIGN_Msk);
	tmpreg |= (SetValue & RTC_ADSIGN_ADSIGN_Msk);
	RTC->ADSIGN = tmpreg;
}
/********************************
时基计数器数值调整方向读取函数
功能:时基计数器数值调整的方向调整读取
输出:	0：表示增加计数初值
	1：表示减少计数初值
********************************/
uint32_t RTC_ADSIGN_ADSIGN_Get(void)
{
	return (RTC->ADSIGN & RTC_ADSIGN_ADSIGN_Msk);
}

/********************************
虚拟调校使能函数（使用PLL虚拟调教）
功能:虚拟调校的功能使能
输入:	0：表示禁止虚拟调校功能
	1：表示使能虚拟调校功能

********************************/
void RTC_PR1SEN_PR1SEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		RTC->PR1SEN |= (RTC_PR1SEN_PR1SEN_Msk);
	}
	else
	{
		RTC->PR1SEN &= ~(RTC_PR1SEN_PR1SEN_Msk);
	}
}
/********************************
获取虚拟调校使能状态函数
功能:获取虚拟调校的功能使能的状态
输出:	使能状态
		0：表示禁止虚拟调校功能
		1：表示使能虚拟调校功能
********************************/
FunState RTC_PR1SEN_PR1SEN_Getable(void)
{
	if (RTC->PR1SEN & (RTC_PR1SEN_PR1SEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
毫秒计数器设置函数（使用PLL虚拟调教）
功能:设置毫秒计数器的数值
输入:	毫秒计数器的数值(以256Hz为周期计数，精度3.9ms )

********************************/
void RTC_MSECCNT_Write(uint32_t SetValue)
{
	RTC->MSECCNT = (SetValue & RTC_MSECCNT_MSCNT_Msk);
}
/********************************
读取毫秒计数器的数值函数（使用PLL虚拟调教）
功能:读取毫秒计数器的数值
输出:	毫秒计数器的数值

********************************/
uint32_t RTC_MSECCNT_Read(void)
{
	return (RTC->MSECCNT & RTC_MSECCNT_MSCNT_Msk);
}

/********************************
PB5触发的时间戳功能使能函数
功能:PB5触发的时间戳功能使能，无复位值，建议软件上电后进行初始化
输入:	1：打开时间戳
	0：关闭时间戳

********************************/
void RTC_STAMPEN_STAMP1EN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		RTC->STAMPEN |= (RTC_STAMPEN_STAMP1EN_Msk);
	}
	else
	{
		RTC->STAMPEN &= ~(RTC_STAMPEN_STAMP1EN_Msk);
	}
}
/********************************
读取PB5触发的时间戳功能使能状态函数
功能:读取PB5触发的时间戳功能使能状态
输出:	1：打开时间戳
	0：关闭时间戳

********************************/
FunState RTC_STAMPEN_STAMP1EN_Getable(void)
{
	if (RTC->STAMPEN & (RTC_STAMPEN_STAMP1EN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
PB4触发的时间戳功能使能函数
功能:PB4触发的时间戳功能使能，无复位值，建议软件上电后进行初始化
输入:	1：打开时间戳
	0：关闭时间戳

********************************/
void RTC_STAMPEN_STAMP0EN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		RTC->STAMPEN |= (RTC_STAMPEN_STAMP0EN_Msk);
	}
	else
	{
		RTC->STAMPEN &= ~(RTC_STAMPEN_STAMP0EN_Msk);
	}
}
/********************************
读取PB4触发的时间戳功能使能状态函数
功能:读取PB4触发的时间戳功能使能状态
输出:	1：打开时间戳
	0：关闭时间戳

********************************/
FunState RTC_STAMPEN_STAMP0EN_Getable(void)
{
	if (RTC->STAMPEN & (RTC_STAMPEN_STAMP0EN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
读取PB4上升沿时间戳时分秒数据函数
功能:读取PB4上升沿时间戳时分秒数据
输出:	PB4上升沿后存储的BCD时分秒数据

********************************/
uint32_t RTC_CLKSTAMP0R_Read(void)
{
	return (RTC->CLKSTAMP0R);
}

/********************************
读取PB4上升沿时间戳年月周日数据函数
功能:读取PB4上升沿时间戳年月周日数据
输出:	PB4上升沿后存储的BCD年月周日数据

********************************/
uint32_t RTC_CALSTAMP0R_Read(void)
{
	return (RTC->CALSTAMP0R);
}


/********************************
读取PB4下升沿时间戳时分秒数据函数
功能:读取PB4下升沿时间戳时分秒数据
输出:	PB4下升沿后存储的BCD时分秒数据

********************************/
uint32_t RTC_CLKSTAMP0F_Read(void)
{
	return (RTC->CLKSTAMP0F);
}

/********************************
读取PB4下升沿时间戳年月周日数据函数
功能:读取PB4下升沿时间戳年月周日数据
输出:	PB4下升沿后存储的BCD年月周日数据

********************************/
uint32_t RTC_CALSTAMP0F_Read(void)
{
	return (RTC->CALSTAMP0F);
}

/********************************
读取PB5上升沿时间戳时分秒数据函数
功能:读取PB5上升沿时间戳时分秒数据
输出:	PB5上升沿后存储的BCD时分秒数据

********************************/
uint32_t RTC_CLKSTAMP1R_Read(void)
{
	return (RTC->CLKSTAMP1R);
}
/********************************
读取PB5上升沿时间戳年月周日数据函数
功能:读取PB5上升沿时间戳年月周日数据
输出:	PB5上升沿后存储的BCD年月周日数据

********************************/
uint32_t RTC_CALSTAMP1R_Read(void)
{
	return (RTC->CALSTAMP1R);
}

/********************************
读取PB5下升沿时间戳时分秒数据函数
功能:读取PB5下升沿时间戳时分秒数据
输出:	PB5下升沿后存储的BCD时分秒数据

********************************/
uint32_t RTC_CLKSTAMP1F_Read(void)
{
	return (RTC->CLKSTAMP1F);
}
/********************************
读取PB5下升沿时间戳年月周日数据函数
功能:读取PB5下升沿时间戳年月周日数据
输出:	PB5下升沿后存储的BCD年月周日数据

********************************/
uint32_t RTC_CALSTAMP1F_Read(void)
{
	return (RTC->CALSTAMP1F);
}

/********************************
RTC所有寄存器清零函数
功能:RTC所有寄存器清零(默认值)
********************************/
void RTC_Deinit(void)
{
	RTC->RTCWE = 0x00000000;
	RTC->RTCIE = 0x00000000;
	//RTC->RTCIF = ;
	//RTC->BCDSEC = ;
	//RTC->BCDMIN = ;
	//RTC->BCDHOUR = ;
	//RTC->BCDDATE = ;
	//RTC->BCDWEEK = ;
	//RTC->BCDMONTH = ;
	//RTC->BCDYEAR = ;
	RTC->ALARM = 0x00000000;
	RTC->FSEL = 0x00000000;
	//RTC->ADJUST = ;
	//RTC->ADSIGN = ;
	RTC->PR1SEN = 0x00000000;
	//RTC->MSECCNT = ;
	RTC->STAMPEN = 0x00000000;
	RTC->CLKSTAMP0R = 0x00000000;
	RTC->CALSTAMP0R = 0x00000000;
	RTC->CLKSTAMP0F = 0x00000000;
	RTC->CALSTAMP0F = 0x00000000;
	RTC->CLKSTAMP1R = 0x00000000;
	RTC->CALSTAMP1R = 0x00000000;
	RTC->CLKSTAMP1F = 0x00000000;
	RTC->CALSTAMP1F = 0x00000000;
}
//Code_End

/********************************
RTC中断使能函数
功能:RTC中断使能
输入：有两个
      一个是使能状态
      另一个是RTC所需要的功能的中断使能
********************************/
void RTC_RTCIE_SetableEx(FunState NewState, uint32_t ie_def)
{
	if (NewState == ENABLE)
	{
		RTC->RTCIE |= (ie_def);
	}
	else
	{
		RTC->RTCIE &= ~(ie_def);
	}
}

/********************************
读取RTC所需要的功能的中断使能状态函数
功能:读取RTC所需要的功能的中断使能状态
输出：中断使能状态
		
********************************/
FunState RTC_RTCIE_GetableEx(uint32_t ie_def)
{
	if (RTC->RTCIE & (ie_def))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/********************************
清除RTC中断标志函数
功能:清除RTC中断标志
输入：要清除的所使用的RTC中断的中断标志
		
********************************/
void RTC_RTCIF_ClrEx(uint32_t if_def)
{
	RTC->RTCIF = (if_def);
}
/********************************
读取所使用的RTC中断标志状态函数
功能:读取RTC中断标志状态
输出：读取所使用的RTC中断标志状态
		
********************************/
FlagStatus RTC_RTCIF_ChkEx(uint32_t if_def)
{
	if (RTC->RTCIF & (if_def))
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 RTC设置时间函数
功能:设置时间，秒，分，时，天，月，年，周，BCD格式
输入：要求设置的时间数组
		
********************************/
void RTC_TimeDate_SetEx(RTC_TimeDateTypeDef* para)
{
	RTC_BCDSEC_Write((uint32_t)para->Second);
	RTC_BCDMIN_Write((uint32_t)para->Minute);
	RTC_BCDHOUR_Write((uint32_t)para->Hour);
	RTC_BCDDATE_Write((uint32_t)para->Date);
	RTC_BCDMONTH_Write((uint32_t)para->Month);
	RTC_BCDYEAR_Write((uint32_t)para->Year);
	RTC_BCDWEEK_Write((uint32_t)para->Week);	
}

/********************************
 RTC读取时间函数
功能:读取时间，秒，分，时，天，月，年，周，BCD格式
输出：读取当前时间，保存为数组
		
********************************/
void RTC_TimeDate_GetEx(RTC_TimeDateTypeDef* para)
{
	para->Second = RTC_BCDSEC_Read();
	para->Minute = RTC_BCDMIN_Read();
	para->Hour = RTC_BCDHOUR_Read();
	para->Date = RTC_BCDDATE_Read();
	para->Month = RTC_BCDMONTH_Read();
	para->Year = RTC_BCDYEAR_Read();
	para->Week = RTC_BCDWEEK_Read();	
}

/********************************
 RTC闹钟设置时间函数
功能:设置闹钟时间，秒，分，时
输入：要求设置的闹钟时间数组
		
********************************/
void RTC_AlarmTime_SetEx(RTC_AlarmTmieTypeDef* para)
{
	RTC_ALARM_ALARMSEC_Set(((uint32_t)para->Second<<RTC_ALARM_ALARMSEC_Pos));
	RTC_ALARM_ALARMMIN_Set(((uint32_t)para->Minute<<RTC_ALARM_ALARMMIN_Pos));
	RTC_ALARM_ALARMHOUR_Set(((uint32_t)para->Hour<<RTC_ALARM_ALARMHOUR_Pos));
}


/********************************
 读取RTC闹钟时间函数
功能:读取闹钟时间，秒，分，时
输出：读取闹钟时间，保存为数组
		
********************************/
void RTC_AlarmTime_GetEx(RTC_AlarmTmieTypeDef* para)
{
	para->Second = (RTC_ALARM_ALARMSEC_Get())>>RTC_ALARM_ALARMSEC_Pos;
	para->Minute = (RTC_ALARM_ALARMMIN_Get())>>RTC_ALARM_ALARMMIN_Pos;
	para->Hour = (RTC_ALARM_ALARMHOUR_Get())>>RTC_ALARM_ALARMHOUR_Pos;
}	
	

/********************************
 读取时间戳数据函数
功能:读取时间戳数据
输出：读取时间戳数据，保存为数组
		
********************************/
void RTC_CLKSTAMPxx_GetEx(uint8_t StampType_def, RTC_StampTmieTypeDef* para)
{
	uint8_t i;
	__IO uint32_t *p_reg = 0;
	p_reg = &(RTC->CLKSTAMP0R);
	
	if(StampType_def <= 3)
	{
		for(i=0; i<StampType_def*2; i++)
		{
			p_reg++;
		}
		para->Second = 0xFF&(p_reg[0]>>0);
		para->Minute = 0xFF&(p_reg[0]>>8);
		para->Hour = 0xFF&(p_reg[0]>>16);
		para->Date = 0xFF&(p_reg[1]>>0);
		para->Month = 0xFF&(p_reg[1]>>16);
		para->Year = 0xFF&(p_reg[1]>>24);
		para->Week = 0xFF&(p_reg[1]>>8);
	}
}

/********************************
 RTC 时钟精度补偿函数
功能:RTC 时钟精度补偿
输入：float型参误差ppm
		
********************************/
void RTC_Trim_Proc(float err_ppm)
{
	float Rtcadj;
	Rtcadj = err_ppm*4.194304;//转换为调校值
		
	if( Rtcadj >= 0 ) 
	{
		RTC_ADJUST_Write((uint32_t)(Rtcadj+0.5));
		RTC_ADSIGN_ADSIGN_Set(RTC_ADSIGN_ADSIGN_PLUS);
	}			
	else              
	{
		RTC_ADJUST_Write((uint32_t)((Rtcadj-0.5)*-1.0));
		RTC_ADSIGN_ADSIGN_Set(RTC_ADSIGN_ADSIGN_MINUS);
	}    
}

/******END OF FILE****/




