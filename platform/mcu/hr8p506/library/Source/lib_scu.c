/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_scu.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  SCU模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_scu.h"

/***************************************************************
  函数名：SCU_NMISelect
  描  述：设置NMI不可屏蔽中断
  输入值：不可屏蔽中断
  输出值：无
  返回值：无
 ***************************************************************/
void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type)
{
	SCU_RegUnLock();
	SCU->NMICON.NMICS = NMI_Type;
	SCU_RegLock() ;
	return;
}

/***************************************************************
  函数名：SCU_GetPWRCFlagStatus
  描  述：获取PWRC复位状态寄存器标志位状态
  输入值：PWRC寄存器标志位
  输出值：无
  返回值：RESET/SET
 ***************************************************************/
FlagStatus  SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag)
{
	FlagStatus bitstatus = RESET;
	SCU_RegUnLock();
	if((SCU->PWRC.Word & (uint32_t)PWRC_Flag) != (uint32_t)RESET)
		bitstatus = SET;
	SCU_RegLock() ;
	return  bitstatus;
}

/***************************************************************
  函数名：SCU_ClearPWRCFlagBit
  描  述：清除PWRC复位状态寄存器标志位
  输入值：PWRC寄存器标志位
  输出值：无
  返回值：无
 ***************************************************************/
void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag)
{
	SCU_RegUnLock() ;
	SCU->PWRC.Word &= ~((uint32_t)PWRC_Flag);
	SCU_RegLock() ;
	return;
}

/***************************************************************
  函数名：SCU_GetLVDFlagStatus
  描  述：获取LVDD寄存器标志位状态
  输入值：LVD寄存器标志位
  输出值：无
  返回值：RESET/SET
 ***************************************************************/
FlagStatus  SCU_GetLVDFlagStatus(SCU_TYPE_LVDCON LVD_Flag)
{
	FlagStatus bitstatus = RESET;
	SCU_RegLock() ;
	if((SCU->LVDCON.Word & (uint32_t)LVD_Flag) != (uint32_t)RESET)
		bitstatus = SET;

	return  bitstatus;
}

/***************************************************************
  函数名：SCU_SysClkSelect
  描  述：选择系统时钟
  输入值：时钟源
  输出值：无
  返回值：无
 ***************************************************************/
void SCU_SysClkSelect(SCU_TYPE_SYSCLK Sysclk)
{
	SCU_RegUnLock() ;
	SCU->SCLKEN0.CLK_SEL = Sysclk;
	SCU_RegLock() ;
	return;
}

/***************************************************************
  函数名：SCU_GetSysClk
  描  述：获取系统时钟源
  输入值：无
  输出值：无
  返回值：系统时钟源
 ***************************************************************/
SCU_TYPE_SYSCLK SCU_GetSysClk(void)
{
	return (SCU_TYPE_SYSCLK)(SCU->SCLKEN0.CLK_SEL);
}

/***************************************************************
  函数名：SCU_OpenXTAL
  描  述: 开启外部时钟
  输入值：无
  输出值：无
  返回值：系统时钟源
 ***************************************************************/
void SCU_OpenXTAL(void)
{
	SCU_RegUnLock();
	SCU->SCLKEN1.XTAL_EN = 1;
	SCU->SCLKEN0.XTAL_LP = 0;
	while(SCU->SCLKEN1.XTAL_RDY == 0); //等待外部时钟稳定
	SCU_RegLock() ;
}

/***************************************************************
  函数名：DeviceClockAllEnable
  描  述：打开所有外设时钟
  输入值：无
  输出值：无
  返回值：无
 ***************************************************************/
void DeviceClockAllEnable(void)
{
	uint32_t prot_tmp;

	prot_tmp = SCU->PROT.PROT;
	if(prot_tmp != 0)   	//写保护了
		SCU_RegUnLock();  	//解锁

	SCU->PCLKEN.Word = 0xFFFFFFFF;     //打开所有外设时钟

	if(prot_tmp != 0)   	//写保护了
		SCU_RegLock();   	//打开写保护

	return;
}

/***************************************************************
  函数名：DeviceClockAllDisable
  描  述：关闭所有外设时钟
  输入值：无
  输出值：无
  返回值：无
 ***************************************************************/
void DeviceClockAllDisable(void)
{
	uint32_t prot_temp;

	prot_temp = SCU->PROT.PROT;
	if(prot_temp != 0)   //写保护了
		SCU_RegUnLock();  //解锁

	SCU->PCLKEN.Word = 0x00000000;     //关闭所有外设时钟，scu无法关闭

	if(prot_temp != 0)   //写保护了
		SCU_RegLock();   //打开写保护

	return;
}

/***************************************************************
  函数名：DeviceClock_Config
  描  述：配置外设是否使能
  输入值：tppe_periph ：选择的外设类型 ，NewState ：Disable或Enable
  输出值：无
  返回值：无
 ***************************************************************/
void DeviceClock_Config(SUC_TYPE_Periph tppe_periph ,TYPE_FUNCEN NewState )
{
	SCU_RegUnLock();
	if(NewState != Disable)
	{
		SCU->PCLKEN.Word |= tppe_periph;
	}
	else
	{
		SCU->PCLKEN.Word &= ~tppe_periph;
	}
	SCU_RegLock(); 
	return;
}


/***************************************************************
  函数名：PLLClock_Config
  描  述：PLL时钟配置,并设置PLL时钟为系统时钟
  输入值：pll_en:是否开启PLL，pll_origin：pll时钟源选择，pll_out：pll输出频率选择，sys_pll：系统时钟是否使用PLL时钟
  输出值：无
  返回值：无
 ***************************************************************/
void PLLClock_Config(TYPE_FUNCEN pll_en , SCU_PLL_Origin  pll_origin ,SCU_PLL_Out pll_out,TYPE_FUNCEN sys_pll )
{
	if(pll_en == Disable)  //如果PLL配置为禁止，则直接禁止PLL，并返回
	{
		SCU->SCLKEN1.PLL_EN = 0;
		return;
	}

	SCU_RegUnLock();
	if((pll_origin==SCU_PLL_IN4M )|| (pll_origin==SCU_PLL_IN16M)) //如果使用内部高速时钟，需开启内部高速时钟
	{
		if(SCU->SCLKEN1.HRC_RDY == 0)
		{
			SCU->SCLKEN1.HRC_EN =1;
			while(SCU->SCLKEN1.HRC_RDY == 0);  //等待HRC开启
		}			

	}
	if((pll_origin==SCU_PLL_OUT4M )|| (pll_origin==SCU_PLL_OUT8M) ||(pll_origin==SCU_PLL_OUT16M )|| (pll_origin==SCU_PLL_OUT32K) )//如果使用外部时钟，需开启外部时钟
	{
		if(SCU->SCLKEN1.XTAL_RDY == 0)
		{
			SCU->SCLKEN1.XTAL_EN =1;
			SCU->SCLKEN0.XTAL_LP = 0;
			while(SCU->SCLKEN1.XTAL_RDY == 0);  //等待XTAL开启
		}			
	}

	switch(pll_origin)
	{
		case SCU_PLL_IN4M:
			SCU->SCLKEN1.PLL_REF_SEL= 0x00;
			break;
		case SCU_PLL_IN16M:
			SCU->SCLKEN1.PLL_REF_SEL= 0x02;
			break;
		case SCU_PLL_IN32K:
			SCU->SCLKEN1.PLL_REF_SEL= 0x03;
			break;
		case SCU_PLL_OUT4M:
			SCU->SCLKEN1.PLL_REF_SEL= 0x04;
			break;
		case SCU_PLL_OUT8M:
			SCU->SCLKEN1.PLL_REF_SEL= 0x05;
			break;
		case SCU_PLL_OUT16M:
			SCU->SCLKEN1.PLL_REF_SEL= 0x06;
			break;
		case SCU_PLL_OUT32K:
			SCU->SCLKEN1.PLL_REF_SEL= 0x07;
			break;
		default:
			break;
	}
	SCU->SCLKEN1.PLL_48M_SEL = pll_out;   //配置PLL输出为32或48Mhz
	SCU->SCLKEN1.PLL_BYLOCK = 0;

	SCU->SCLKEN1.PLL_EN = 1;
	while(SCU->SCLKEN1.PLL_RDY == 0);
	if(sys_pll == Enable)
	{
		SCU->SCLKEN0.PLL_MUX = 1;
	}
	else
	{
		SCU->SCLKEN0.PLL_MUX = 0;
	}


	SCU_RegLock(); 
}


/***************************************************************
  函数名：SCU_MultTimerEnable
  描  述：多个时钟同时开启(T16N0/1/2/3,P32N0)
  输入值：SCU_TimerMask：可以是T16N0/1/2/3和T32N0的任意组合
  输出值：无
  返回值：无
 ***************************************************************/
void SCU_MultTimerEnable(SCU_TIMEREN_Typedef SCU_TimerMask)
{
	SCU_RegUnLock();
	SCU->TIMEREN.Word = SCU_TimerMask.Word;
	SCU_RegLock();
}


/***************************************************************
  函数名：SCU_MultTimerDisable
  描  述：多个时钟同时关闭(T16N0/1/2/3,P32N0)
  输入值：SCU_TimerMask：可以是T16N0/1/2/3和T32N0的任意组合
  输出值：无
  返回值：无
 ***************************************************************/
void SCU_MultTimerDisable(SCU_TIMERDIS_Typedef SCU_TimerMask)
{
	SCU_RegUnLock();
	SCU->TIMERDIS.Word = SCU_TimerMask.Word;
	SCU_RegLock();
}



