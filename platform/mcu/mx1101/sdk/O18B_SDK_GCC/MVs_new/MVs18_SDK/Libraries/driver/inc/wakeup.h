/**
 *******************************************************************************
 * @file    wakeup.h
 * @author  lujiangang
 * @version V1.0.0
 * @date    27-May-2013
 * @brief   wakeup driver header file
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */


#ifndef __WAKEUP_H__
#define __WAKEUP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"



/**
 * WakeUp Flag.
 */
#define WAKEUP_FLAG_POR_LDOIN		0x00001
#define WAKEUP_FLAG_WDG				0x00002
#define WAKEUP_FLAG_RTC				0x00004		//RTC闹钟定时唤醒(RTC Alarm WakeUp)
#define WAKEUP_FLAG_POWERKEY		0x00008		//POWER-KEY唤醒(POWER-KEY WakeUp)
#define WAKEUP_FLAG_SYSRST			0x00010		//软复位(System Reset)
#define WAKEUP_FLAG_RSTPIN			0x00020
#define WAKEUP_FLAG_GPIOA10			0x00040
#define WAKEUP_FLAG_GPIOB5			0x00080
#define WAKEUP_FLAG_GPIOB6			0x00100
#define WAKEUP_FLAG_GPIOB7			0x00200
#define WAKEUP_FLAG_GPIOB22			0x00400
#define WAKEUP_FLAG_GPIOB23			0x00800
#define WAKEUP_FLAG_GPIOB24			0x01000
#define WAKEUP_FLAG_GPIOB25			0x02000
#define WAKEUP_FLAG_GPIOC2			0x04000
#define WAKEUP_FLAG_GPIOC7			0x08000
#define WAKEUP_FLAG_GPIOC8			0x10000
#define WAKEUP_FLAG_GPIOC9			0x20000
#define WAKEUP_FLAG_POR_RTC			0x040000	//RTC闹钟定时开机(RTC Alarm WakeUp Core)
#define WAKEUP_FLAG_POR_POWERKEY	0x080000	//POWER-KEY开机(POWER-KEY PowerOn)
#define WAKEUP_FLAG_POR_8SRST		0x100000	//POWER-KEY 8S复位(POWER-KEY 8S Reset)
#define WAKEUP_FLAG_POR_POWER		0x200000	//POWER ON(保持POWER_KEY ON的情况下，由电源开机)

/**
 * WakeUp Source(in deep Sleep mode) Select.
 */
#define WAKEUP_SRC_SLEEP_POWERKEY	0x0001
#define WAKEUP_SRC_SLEEP_RTC		0x0002
#define WAKEUP_SRC_SLEEP_A10		0x0004
#define WAKEUP_SRC_SLEEP_B5			0x0008
#define WAKEUP_SRC_SLEEP_B6			0x0010
#define WAKEUP_SRC_SLEEP_B7			0x0020
#define WAKEUP_SRC_SLEEP_B22		0x0040
#define WAKEUP_SRC_SLEEP_B23		0x0080
#define WAKEUP_SRC_SLEEP_B24		0x0100
#define WAKEUP_SRC_SLEEP_B25		0x0200
#define WAKEUP_SRC_SLEEP_C2			0x0400
#define WAKEUP_SRC_SLEEP_C7			0x0800
#define WAKEUP_SRC_SLEEP_C8			0x1000
#define WAKEUP_SRC_SLEEP_C9			0x2000
/**
 * WakeUp Source(in power-down) Select.
 */
#define WAKEUP_SRC_PD_POWERKEY		0x2000
#define WAKEUP_SRC_PD_RTC			0x4000

/**
 * Pin(GPIO、POWER-KEY WakeUp) wakeUp Trig Polarity(1'b0: low wakeup, 1'b1: high wakeup).
 */
#define WAKEUP_POLAR_POWERKEY_LOW   0
#define WAKEUP_POLAR_POWERKEY_HI	0x0001
#define WAKEUP_POLAR_A10_LOW		0
#define WAKEUP_POLAR_A10_HI			0x0002
#define WAKEUP_POLAR_B5_LOW			0
#define WAKEUP_POLAR_B5_HI			0x0004
#define WAKEUP_POLAR_B6_LOW			0
#define WAKEUP_POLAR_B6_HI			0x0008
#define WAKEUP_POLAR_B7_LOW			0
#define WAKEUP_POLAR_B7_HI			0x0010
#define WAKEUP_POLAR_B22_LOW		0
#define WAKEUP_POLAR_B22_HI			0x0020
#define WAKEUP_POLAR_B23_LOW		0
#define WAKEUP_POLAR_B23_HI			0x0040
#define WAKEUP_POLAR_B24_LOW		0
#define WAKEUP_POLAR_B24_HI			0x0080
#define WAKEUP_POLAR_B25_LOW		0
#define WAKEUP_POLAR_B25_HI			0x0100
#define WAKEUP_POLAR_C2_LOW			0
#define WAKEUP_POLAR_C2_HI			0x0200
#define WAKEUP_POLAR_C7_LOW			0
#define WAKEUP_POLAR_C7_HI			0x0400
#define WAKEUP_POLAR_C8_LOW			0
#define WAKEUP_POLAR_C8_HI			0x0800
#define WAKEUP_POLAR_C9_LOW			0
#define WAKEUP_POLAR_C9_HI			0x1000

/**
 * Pin(GPIO、POWER-KEY) wakeup minimal time mode(0: no minimal time limitation, 1: minimal time is 1ms).
 */
#define WAKEUP_TMODE_0MS			0
#define WAKEUP_TMODE_1MS			1

/**
 * POWER-KEY mode.
 */
#define POWERKEY_MODE_BYPASS		0
#define POWERKEY_MODE_PUSH_BUTTON	1	//按钮开关(软开关)
#define POWERKEY_MODE_SLIDE_SWITCH	2	//拨动开关(硬开关)


/**
 * global wakeup flag.
 */
extern uint32_t gWakeUpFlag;


/**
 * @brief  Put the system into sleep mode.
 * @param  None.
 * @return None.
 * @Note   This function will disabel systick timer(for in OS system, the systick will wakeup system)
 */
//void SysSleep(void);

/**
 * @brief  detect whether the  power-key is triggered.
 * @param  None.
 * @return  return TRUE if power key is triggered, otherwise return FALSE.
 * @note IN Powerkey HARD Mode, system should be power down when "powerkey=0" lasts for a certain time, 
 * @ 						  so the time required to eliminate keystroke jitter should be concerned by software
 * @       IN Powerkey SOFT Mode, for system will be powered down by internal register state not "powerkey=0", 
 * @                                             so hardware will eliminate the keystroke jitter automatically
 */
bool PowerKeyDetect(void);
	
/**
 * @brief  Initialize the power-key after reset or power on.
 * @param  PowerKeyMode -> POWER-KEY mode.
 * @param SwitchOnTime -> Setting a threshold value for POWER-KEY Press(Unit:ms).
 * @note	   The SysGetWakeUpSrc() function must be executed previously.
 * 		   This function must be called before SysSetWakeUpSrcInPowerDown.
 * @  SwitchOnTime setting reference:
 * @		   IN Powerkey HARD Mode 500ms.
 * @		   IN Powerkey SOFT Mode, 2000ms.
 */
void SysPowerKeyInit(uint8_t PowerKeyMode, uint16_t SwitchOnTime);


/**
 * @brief  Put the system into deep sleep mode.
 * @param  None.
 * @return None.
 * @pre	   The SysSetWakeUpSrcInDeepSleep() function must be executed previously.
 * @Note   This function will set Global Wakeup Enable.
 *		  this function will power down analog module
 */
void SysGotoDeepSleep(void);


/**
 * @brief  Put the system into power-down mode.
 * @param  None.
 * @return None.
 * @pre	   The SysPowerKeyInit & SysSetWakeUpSrcInPowerDown functions must be executed previously.
 * @note   This function is invalid only for POWERKEY_MODE_BYPASS mode.
 */
void SysGotoPowerDown(void);


/**
 * @brief  Set the wakeup source in deep sleep mode.
 * @param  WakeSrc -> the source for wakeup in deep sleep.
 * @param  Polarity-> the polarity of the wakeup-source(GPIO、ROWER-KEY).
 * @param  Tmode   -> Pin(GPIO、POWER-KEY) wakeup minimal time mode(0: no minimal time limitation, 1: minimal time is 1ms).
 * @return None.
 */
void SysSetWakeUpSrcInDeepSleep(uint32_t WakeSrc, uint32_t Polarity, bool Tmode);


/**
 * @brief  Set the wakeup source in power-down mode.
 * @param  WakeSrc -> the source for wakeup in power-down.
 * @return None.
 */
void SysSetWakeUpSrcInPowerDown(uint32_t WakeSrc);


/**
 * @brief  Clear the source for wakeup(both deep sleep and power-down).
 * @param  WakeSrc -> the souce for wakeup(both deep sleep and power-down).
 * @return None.
 */
void SysClrWakeUpSrc(uint32_t WakeSrc);


/**
 * @brief  Get the wakeup source(flag).
 * @param  None.
 * @return Return the wakeup-flag,which can indicate the source of wakeup or reset.
 * @Note   a). This function must be called before SysPowerKeyInit、SysSetWakeUpSrcInDeepSleep and SysSetWakeUpSrcInPowerDown.
 *		   b). After this function is used,all the wakeup sources of sleep was disabled(global_wakeup_enable bit is cleared).
 *		   c). After this function is used,the wakeup-flag is to be cleard.
 */
uint32_t SysGetWakeUpFlag(void);

/**
 * @Brief:	Get pad_onkey status is pressed or not
 * @Param:	None
 * @Return:	TURE or FALSE
 * @Note@:
 */
bool PowerkeyGetOnkeyReg(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__WAKEUP_H__
