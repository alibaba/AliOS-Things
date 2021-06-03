/**
 * ant_timer.h
 *
 * 定时器接口
 */
#ifndef __ANT_TIMER_H__
#define __ANT_TIMER_H__

#include "ant_typedef.h"
#include "ant_os.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define TIME_SYNC_TIMER       0
#define LED_BLINK_TIMER       1
#define TIMER_ID_MAX 5

typedef void (*callback_on_timer)(void *param);
typedef void * ant_timer;

/**
 * 定时器模块初始化，系统初始化时需要调用的接口
 *
 * @return  0: 成功
 *         -1: 失败
 */
ant_s32   ant_timer_init(void);

/**
 * 创建定时器
 *
 * @return 成功: 返回定时器对象
 *         失败: 返回NULL
 */
ant_timer ant_timer_create(void);

/**
 * 启动定时器
 *
 * @param timer 定时器对象
 * @param callback 时间到达时的回调函数
 * @param param 回调函数参数
 * @param interval 定时时间
 * @param resched_interval 定时时间发生时，自动重新开启定时的时间，如果传入0，则定时器自动停止
 */
void      ant_timer_start(ant_timer timer, callback_on_timer callback, void *param, ant_u32 interval, ant_u32 resched_interval);

/**
 * 停止定时器对象
 *
 * @param timer 定时器对象
 */
void      ant_timer_stop(ant_timer timer);

/**
 * 获取当前定时器剩余时间
 *
 * @param timer 定时器对象
 * @return 剩余时间，单位为毫秒
 */
ant_u32   ant_timer_get_remaining_time(ant_timer timer);

/**
 * 销毁定时器
 *
 * @param timer 定时器对象
 */
void      ant_timer_destroy(ant_timer timer);

#ifdef __cplusplus
}
#endif

#endif

