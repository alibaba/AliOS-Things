/*
* timer_api.h
* timer API for application module
* auther: lean.xiong @2011-09-13
* notes: 软件Timer是一个低精度的定时器，定时间隔不得小于MIN_TIMER_PERIOD
* maintainer lujiangang
*/
#ifndef __TIMER_API_H__
#define __TIMER_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define APP_TIMER_API // 对OS Timer 的封装


#ifdef APP_TIMER_API

unsigned int xTaskGetTickCount(void);

#define MIN_TIMER_PERIOD 50/*ms*/ // 最小定时间隔

#define MAX_SYS_TICKS 0xFFFFFFFF // 系统Ticks最大值，用于计时翻转

typedef void (*TIMER_CALLBACK)(void* Param);

#pragma pack(1)

typedef struct _SW_TIMER_
{
	uint32_t TimerPeriod;  // ms,【注意】最小定时间隔与应用模式主循环的其他功能阻塞时间有关，建议不要低于主循环的最大阻塞时间；
	uint32_t LastTickets;  // 用于timeout判断
	uint32_t InitTicks;    // Timer第一次开启时的ticks
	uint32_t PausedTimes;  // ms，已暂停的时间
	TIMER_CALLBACK Callback;
	uint8_t  IsRunning;
	uint8_t  TicksResetCount; // 记录SysTicks翻转次数
} SW_TIMER;

#pragma pack()

// 获得当前Timer实际运行时间，单位ms
uint32_t GetPassTime(SW_TIMER* TimerHandle);

// 应用模式的循环中，调用该函数，轮询Timer（定时调用CallBack）
void CheckTimer(SW_TIMER* TimerHandle);

// 初始化Timer
bool InitTimer(SW_TIMER* TimerHandle,
	               uint32_t TimerPeriod, /*定时间隔ms*/
	               TIMER_CALLBACK CallbackFunc/*定时回调函数*/);

// 启动Timer
bool StartTimer(SW_TIMER* TimerHandle);

// 停止Timer
bool StopTimer(SW_TIMER* TimerHandle);

// 销毁Timer
bool DeinitTimer(SW_TIMER* TimerHandle);

// 重置Timer的计时状态，【注意】清除以前的值，如实际运行时间，Timer启动时间点等
bool ResetTimer(SW_TIMER* TimerHandle);

// 更改Timer定时间隔
bool ChangeTimerPeriod(SW_TIMER* TimerHandle, uint32_t TimerPeriod/*定时间隔*/);

#endif/*SYS_TIMER_API*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*TIMER_API_H*/

