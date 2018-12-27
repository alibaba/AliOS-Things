/**
 *******************************************************************************
 * @file    time_api.h
 * @author  lujiangang
 * @version V1.0.1
 * @date    27-May-2013
 * @brief   timer API header file
 * @note 软件Timer是一个低精度的定时器，定时间隔不得小于MIN_TIMER_PERIOD
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
 
/**
* @addtogroup 驱动
* @{
* @defgroup TimerAPI TimerAPI
* @{
*/
 
#ifndef __TIMER_API_H__
#define __TIMER_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define APP_TIMER_API 	/**<对OS Timer 的封装 */


#ifdef APP_TIMER_API

unsigned int xTaskGetTickCount(void);

#define MIN_TIMER_PERIOD 50				/**<最小定时间隔ms */ 

#define MAX_SYS_TICKS 0xFFFFFFFF 	/**<系统Ticks最大值，用于计时翻转 */

typedef void (*TIMER_CALLBACK)(void* Param);

#pragma pack(1)

typedef struct _SW_TIMER_
{
	uint32_t TimerPeriod;  /**< ms,注意，最小定时间隔与应用模式主循环的其他功能阻塞时间有关，建议不要低于主循环的最大阻塞时间*/
	uint32_t LastTickets;  /**< 用于timeout判断*/
	uint32_t InitTicks;    /**<Timer第一次开启时的ticks*/
	uint32_t PausedTimes;  /**<ms，已暂停的时间*/
	TIMER_CALLBACK Callback;	
	uint8_t  IsRunning;				
	uint8_t  TicksResetCount; /**<记录SysTicks翻转次数*/
} SW_TIMER;

#pragma pack()

/**
 * @brief  获得当前Timer实际运行时间，单位ms
 * @param  *TimerHandle	Timer句柄
 * @return Timer运行时间
 */
uint32_t GetPassTime(SW_TIMER* TimerHandle);

/**
 * @brief  应用模式的循环中，调用该函数，轮询Timer（定时调用CallBack）
 * @param  *TimerHandle Timer句柄
 * @return None
 */
void CheckTimer(SW_TIMER* TimerHandle);

/**
 * @brief  初始化Timer
 * @param  *TimerHandle	Timer句柄
 * @param TimerPeriod  	定时间隔ms
 * @param CallbackFunc	 定时回调函数
 * @return 成功 TRUE，失败FALSE
 */
bool InitTimer(SW_TIMER* TimerHandle,
	               uint32_t TimerPeriod,
	               TIMER_CALLBACK CallbackFunc);

/**
 * @brief  启动Timer
 * @param  *TimerHandle Timer句柄
 * @return 成功TRUE，失败FALSE
 */
bool StartTimer(SW_TIMER* TimerHandle);

/**
 * @brief  停止Timer
 * @param  *TimerHandle	Timer句柄
 * @return 成功TRUE，失败FALSE
 */
bool StopTimer(SW_TIMER* TimerHandle);


/**
 * @brief  销毁Timer
 * @param  *TimerHandle:Timer句柄
 * @return 成功TRUE，失败FALSE
 */
bool DeinitTimer(SW_TIMER* TimerHandle);

/**
 * @brief  重置Timer的计时状态
 * @param  *TimerHandle	Timer句柄
 * @return 成功TRUE，失败FALSE
 * @note 清除以前的值，如实际运行时间，Timer启动时间点等
 */
bool ResetTimer(SW_TIMER* TimerHandle);

/**
 * @brief  更改Timer定时间隔
 * @param  *TimerHandle	Timer句柄
 * @param  TimerPeriod	定时间隔
 * @return 成功TRUE，失败FALSE
 */
bool ChangeTimerPeriod(SW_TIMER* TimerHandle, uint32_t TimerPeriod);

#endif/*SYS_TIMER_API*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*TIMER_API_H*/

/**
 * @}
 * @}
 */
