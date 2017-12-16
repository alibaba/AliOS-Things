///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: power_management.c
//  maintainer: lujiangang
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "app_config.h"
#include "uart.h"
#include "adc.h"
#include "clk.h"
#include "gpio.h"
#include "wakeup.h"
#include "sys_app.h"

#ifdef FUNC_POWER_MONITOR_EN

#define LDOIN_SAMPLE_COUNT			10		//获取LDOIN幅度时用来平均的采样次数
#define LDOIN_SAMPLE_PERIOD			50		//获取LDOIN幅度时获取采样值的间隔(ms)
#define LOW_POWEROFF_TIME			10000		//低电检测关机连续检测时间(ms)


//以下定义不同的电压检测事件的触发电压(单位mV)，用户根据自身系统电池的特点来配置
#define LDOIN_VOLTAGE_FULL			4200
#define LDOIN_VOLTAGE_HIGH			3800
#define LDOIN_VOLTAGE_MID			3600
#define LDOIN_VOLTAGE_LOW			3500
#define LDOIN_VOLTAGE_OFF			3300	//低于此电压值进入关机powerdown状态

//电压检测时不同的显示处理
typedef enum _PWR_MNT_DISP
{
	PWR_MNT_DISP_NONE = 0,
	PWR_MNT_DISP_CHARGE,		 
	PWR_MNT_DISP_HIGH_V, 
	PWR_MNT_DISP_MID_V, 
	PWR_MNT_DISP_LOW_V, 
	PWR_MNT_DISP_EMPTY_V, 
	PWR_MNT_DISP_SYS_OFF
	 
} PWR_MNT_DISP;

//主要用于电压状态显示的变量
static TIMER BlinkTimer;
#ifdef	OPTION_CHARGER_DETECT
static PWR_MNT_DISP PwrMntDisp;
#endif


//用于电压检测的变量
TIMER PowerMonitorTimer;
uint32_t LdoinSampleSum = 0; 
uint16_t  LdoinSampleCnt = LDOIN_SAMPLE_COUNT;
uint32_t LdoinLevelAverage = 0;		//当前LDOIN电压平均值


#ifdef	OPTION_CHARGER_DETECT
//硬件检测PC 或充电器连接状态
//使能内部下拉。无PC或充电器连接时，检测口为低电平，有时检测口为高电平
bool IsInCharge(void)
{
//设为输入，无上下拉

	GpioSetRegBits(CHARGE_DETECT_PORT_PU, CHARGE_DETECT_BIT);
	GpioSetRegBits(CHARGE_DETECT_PORT_PD, CHARGE_DETECT_BIT);
	GpioClrRegBits(CHARGE_DETECT_PORT_OE, CHARGE_DETECT_BIT);	   
	GpioSetRegBits(CHARGE_DETECT_PORT_IE, CHARGE_DETECT_BIT);
	WaitUs(2);
	if(GpioGetReg(CHARGE_DETECT_PORT_IN) & CHARGE_DETECT_BIT)
	{
		return TRUE;
	}   	

	return FALSE;
}
#endif
void PowerMonitorDisp(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) ||defined(FUNC_TM1628_LED_EN))
	static uint8_t  ShowStep = 0;
	static bool IsToShow = FALSE;

	switch(PwrMntDisp)
	{			
		case PWR_MNT_DISP_CHARGE:
			//闪烁充电ICON,表示正在充电
			if(IsTimeOut(&BlinkTimer))
			{
				TimeOutSet(&BlinkTimer, 500);
				switch(ShowStep)
				{

					case 0:
						DispIcon(ICON_BAT1, FALSE);
						DispIcon(ICON_BAT2, FALSE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 1:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, FALSE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 2:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, TRUE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 3:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, TRUE);
						DispIcon(ICON_BAT3, TRUE);
						break;
				}		
				if(ShowStep < 3)
				{
					ShowStep++;
				}
				else
				{
					ShowStep = 0;
				}
			}
			
			break;

		case PWR_MNT_DISP_HIGH_V:
			//DBG("BAT FULL\n");			
			DispIcon(ICON_BATFUL, TRUE);
			DispIcon(ICON_BATHAF, FALSE);
			//可以显示满格电量，请添加显示代码
			break;
			
		case PWR_MNT_DISP_MID_V:
			//DBG("BAT HALF\n");		
			DispIcon(ICON_BATFUL, FALSE);
			DispIcon(ICON_BATHAF, TRUE);
			//可以显示2格电量，请添加显示代码
			break;

		case PWR_MNT_DISP_LOW_V:
			DispIcon(ICON_BATFUL, FALSE);
			DispIcon(ICON_BATHAF, TRUE);
			//可以显示1格电量，请添加显示代码
			break;
			
		case PWR_MNT_DISP_EMPTY_V:
			//DBG("BAT EMPTY\n");				
			DispIcon(ICON_BATFUL, FALSE);

			if(IsTimeOut(&BlinkTimer))
			{
				TimeOutSet(&BlinkTimer, 300);
				if(IsToShow)
				{
					DispIcon(ICON_BATHAF, TRUE);
				}
				else
				{
					DispIcon(ICON_BATHAF, FALSE);
				}
				IsToShow = !IsToShow;
			}
			//可以显示0格电量，请添加显示代码
			break;
		
		case PWR_MNT_DISP_SYS_OFF:
			//DBG("BAT OFF\n");
			//ClearScreen();			//清除显示				
			//DispString(" LO ");
			break;
			
		default:
			break;
	}
#endif	
}
//监测LDOIN的电压值，执行对应检测点的处理
//PowerOnInitFlag: TRUE--第一次上电执行电源监控检测
static void PowerLdoinLevelMonitor(bool PowerOnInitFlag)
{	
	//bool PowerOffFlag = FALSE;
	//uint16_t AdcVal;	
	if(LdoinSampleCnt > 0)
	{
		LdoinSampleSum += SarAdcGetLdoinVoltage();		
		LdoinSampleCnt--;
	}

	//采样够LDOIN_SAMPLE_COUNT次数，求LDOIN平均值
	if(LdoinSampleCnt == 0)
	{
		LdoinLevelAverage = LdoinSampleSum / LDOIN_SAMPLE_COUNT;

//		DBG("LDOin 5V Volt: %lu\n", (uint32_t)LdoinLevelAverage);

		//为继续检测LDOIN，初始化变量
		LdoinSampleCnt = LDOIN_SAMPLE_COUNT;
		LdoinSampleSum = 0;

#ifdef	OPTION_CHARGER_DETECT
		if(IsInCharge())		//充电器已经接入的处理
		{		
			PowerMonitorDisp();
			return;
		}

		
		if(LdoinLevelAverage > LDOIN_VOLTAGE_HIGH)	  
		{
			//可以显示满格电量，请在PowerMonitorDisp中添加显示代码
			//PowerMonitorDisp(PWR_MNT_DISP_HIGH_V);
			PwrMntDisp = PWR_MNT_DISP_HIGH_V;
			//DBG("bat full\n");
		}

		else if(LdoinLevelAverage > LDOIN_VOLTAGE_MID)
		{
			//可以显示2格电量，请在PowerMonitorDisp中添加显示代码
			//PowerMonitorDisp(PWR_MNT_DISP_MID_V);
			PwrMntDisp = PWR_MNT_DISP_MID_V;
		}
		else if(LdoinLevelAverage > LDOIN_VOLTAGE_LOW)
		{
			//可以显示1格电量，请在PowerMonitorDisp中添加显示代码
			//PowerMonitorDisp(PWR_MNT_DISP_LOW_V);
			PwrMntDisp = PWR_MNT_DISP_LOW_V;
			//DBG("bat LOW\n");
		}
		else if(LdoinLevelAverage > LDOIN_VOLTAGE_OFF)
		{
			//可以显示0格电量，请在PowerMonitorDisp中添加显示代码
			//PowerMonitorDisp(PWR_MNT_DISP_EMPTY_V);
			PwrMntDisp = PWR_MNT_DISP_EMPTY_V;
		}
#endif
		

		if(LdoinLevelAverage <= LDOIN_VOLTAGE_OFF)
		{
			//低于关机电压，进入关机流程
			//可以先行显示低电压提示，然后执行关机动作
#ifdef	OPTION_CHARGER_DETECT			
			PwrMntDisp = PWR_MNT_DISP_SYS_OFF;
			PowerMonitorDisp();
#endif			
			//WaitMs(1000);
			//停止正常工作流程，包括关显示、关DAC、关功放电源等动作
			DBG("PowerMonitor, Low Voltage!PD.\n");	

			
			//low level voltage detect in power on sequence, power down system directly
			if(PowerOnInitFlag == TRUE)
			{
				SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_POWERKEY);
				SysGotoPowerDown(); 
			}
			else	// PowerOnInitFlag == FALSE
			{
				if(gSys.NextModuleID != MODULE_ID_POWEROFF)	/*MSG_COMMON_CLOSE only need send once*/
				{
					/*if use push button, send message, for task's saving info.*/
					APP_DBG("Low level voltage detected->send message common close\n");
					gSys.NextModuleID = MODULE_ID_POWEROFF;
					MsgSend(MSG_COMMON_CLOSE);
				}
			}
		}		
	}
	PowerMonitorDisp();
}


//电能监视初始化
//实现系统启动过程中的低电压检测处理，以及初始化充电设备接入检测IO等
void PowerMonitorInit(void)
{
	TimeOutSet(&PowerMonitorTimer, 0);	
	TimeOutSet(&BlinkTimer, 0);	
#ifdef OPTION_CHARGER_DETECT
	//如果系统启动时，充电设备已经接入，就不执行下面这段低电压检测和处理过程
	if(!IsInCharge())
#endif
	{
#ifdef POWERON_DETECT_VOLTAGE	// 开机检测电压
		//系统启动过程中的低电压检测
		//开机时电压检测，如果小于开机电压，不进入设备检测和播放流程，直接关机
		//检测过程为时50ms，并进行对应的处理
		while(LdoinSampleCnt)
		{
			LdoinSampleCnt--;
			LdoinSampleSum += SarAdcGetLdoinVoltage();
			WaitMs(5);
		}		
		//为后边的LDOIN检测初始化变量
		PowerLdoinLevelMonitor(TRUE);
#endif
	}
}

//系统电源状态监控和处理
//系统启动后如果LDOIN大于开机电压，放在系统正常运行中监测LDOIN
void PowerMonitor(void)
{
	if(IsTimeOut(&PowerMonitorTimer))
	{
		TimeOutSet(&PowerMonitorTimer, LDOIN_SAMPLE_PERIOD);

#ifdef OPTION_CHARGER_DETECT
		if(IsInCharge())		//充电器已经接入的处理
		{
			if(LdoinLevelAverage >= LDOIN_VOLTAGE_FULL) 
			{
				//PowerMonitorDisp(PWR_MNT_DISP_HIGH_V);		//显示充电状态
				PwrMntDisp = PWR_MNT_DISP_HIGH_V;
				//DBG("charger full\n");
			}
			else
			{
				//PowerMonitorDisp(PWR_MNT_DISP_CHARGE);		//显示充电状态
				PwrMntDisp = PWR_MNT_DISP_CHARGE;
				//DBG("charger.....\n");
			}
		}
#endif		
		//没有采样够LDOIN_SAMPLE_COUNT次数，继续采样
		PowerLdoinLevelMonitor(FALSE);
	}
}
#ifdef LOW_POWER_SAVE_PLAYTIME_TO_FLASH
extern uint32_t AudioPlaySavePlayTimeToFlash(void);

void LowPowerDetProc(void)
{
	uint16_t RetVal;

	RetVal = SarAdcGetLdoinVoltage();

	if((RetVal < 4200) && (RetVal > 3400))	
   	{
	   	AudioPlaySavePlayTimeToFlash();
   	}
}
#endif
#endif	//end of FUNC_POWER_MONITOR_EN

/**
 * @brief 		system power off detect function
 *			this function should be called in timer INT function or systick function for periodically detecting
 *			when USE_POWERKEY_SLIDE_SWITCH, the system will be powered off directly
 *			when USE_POWERKEY_SOFT_PUSH_BUTTON, it wil send message:MSG_COMMON_CLOSE, for task's saving information, 
* 			and then power off system
 * @param	None
 * @return	None
 */
void SystemPowerOffDetect(void)
{

#ifdef USE_POWERKEY_SLIDE_SWITCH
#define SLIDE_SWITCH_ONTIME 500
	static uint16_t  slide_switch_pd_cnt = SLIDE_SWITCH_ONTIME;//消抖时间0.5s，见PowerKeyDetect()描述
	if(PowerKeyDetect())
	{
		if(slide_switch_pd_cnt-- == 0)
		{
			/*if slide switch, power down system directly*/
			APP_DBG("PowerKeyDetect->go to PowerDown\n");
			SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_POWERKEY);
			SysGotoPowerDown(); 
			while(1);
		}
	}
	else
	{
		slide_switch_pd_cnt = SLIDE_SWITCH_ONTIME;
	}
#endif 
	
#ifdef USE_POWERKEY_SOFT_PUSH_BUTTON
	if(PowerKeyDetect())
	{
		if(gSys.NextModuleID != MODULE_ID_POWEROFF)	/*MSG_COMMON_CLOSE only need send once*/
		{
			/*if use push button, send message, for task's saving info.*/
			APP_DBG("PowerKeyDetect->send message common close\n");
			gSys.NextModuleID = MODULE_ID_POWEROFF;
			MsgSend(MSG_COMMON_CLOSE);
		}
	}
#endif	
	
}

// 进入deep sleep，为了降低功耗，默认设置为输入上拉。
// 客户可以根据需要进行一些定制化配置
void SysGotoDeepSleepGpioCfg(void)
{
	GpioSetRegBits(GPIO_A_IE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_A_OE, 0xFFFFFFFF);
	GpioSetRegBits(GPIO_A_PU, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_A_PD, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_B_IE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_B_OE, 0xFFFFFFFF);
	GpioSetRegBits(GPIO_B_PU, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_B_PD, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_C_IE, 0x7FFF);
	GpioClrRegBits(GPIO_C_OE, 0x7FFF);
	GpioSetRegBits(GPIO_C_PU, 0x7FFF);
	GpioClrRegBits(GPIO_C_PD, 0x7FFF);

	// BT close, not arbitrarily modify
	//BTDevicePowerOff();
}


/**
 * @brief 		system power off flow
 *			user can add other functions before SysGotoPowerDown()
 *			
 * @param	None
 * @return	None
 */
void SystemPowerOffControl(void)
{
	APP_DBG("SystemPowerOffControl->system will power off soon!\n");
	
	SysVarDeinit();
	//APP_DBG("REG_LP_WAKEUP = %d, s_onoff = %d\n", (*(volatile unsigned long*)0x4002200C), PowerkeyGetSOnOff());
	//SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_POWERKEY);
	SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_RTC);
	SysGotoPowerDown(); 

	/*Never reach here,  expect power down fail*/
	APP_DBG("System Power Down Fail!");
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(1);
}

/**
 * @brief 		system standby flow
 *			user can add other functions before SysGotoDeepSleep()
 *			
 * @param	None
 * @return	None
 */
void SystemStandbyControl(void)
{
	uint32_t Port = GPIOC7;
	APP_DBG("StandBy Mode\n");
	SysVarDeinit();
	
	SysGotoDeepSleepGpioCfg();

	//set GPIOB24 input enable, pull down
	GpioSetRegOneBit(GPIO_C_IE, Port);
	GpioSetRegOneBit(GPIO_C_PU, Port);
	GpioSetRegOneBit(GPIO_C_PD, Port);  

	//set wakeup source GPIOB24, Wakeup Polarith: High wakeup, pin wakeup valid minimal time is 1ms
	SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_C7, WAKEUP_POLAR_C7_HI, WAKEUP_TMODE_1MS);
	SysGotoDeepSleep();

	/*Never reach here,  expect go to deepsleep fail*/
	APP_DBG("System Go to Deep Sleep Fail!");
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(1);
}
