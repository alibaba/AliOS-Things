///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: power_management.h
//  maintainer: lujiangang
///////////////////////////////////////////////////////////////////////////////

#ifndef __POWER_MANAGEMENT_H__
#define __POWER_MANAGEMENT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief 		system power off detect function
 *			this function should be called in timer INT function or systick function for periodically detecting
 *			when USE_POWERKEY_SLIDE_SWITCH, the system will be powered off directly
 *			when USE_POWERKEY_SOFT_PUSH_BUTTON, it wil send message:SYS_MSG_COMMON_CLOSE, for task's saving information, 
* 			and then power off system
 * @param	None
 * @return	None
 */
void SystemPowerOffDetect(void);

void SysGotoDeepSleepGpioCfg(void);


/**
 * @brief 		system power off flow
 *			user can add other functions before SysGotoPowerDown()
 *			
 * @param	None
 * @return	None
 */
void SystemPowerOffControl(void);

/**
 * @brief 		system standby flow
 *			user can add other functions before SysGotoDeepSleep()
 *			
 * @param	None
 * @return	None
 */
void SystemStandbyControl(void);


//电能监视初始化
//实现系统启动过程中的低电压检测处理，以及配置充电设备接入检测IO等
//
void PowerMonitorInit(void);

//
//系统电源状态监控和处理
//系统启动后如果LDOIN大于开机电压，放在系统正常运行中监测LDOIN
//
void PowerMonitor(void);

void PowerMonitorDisp(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif



