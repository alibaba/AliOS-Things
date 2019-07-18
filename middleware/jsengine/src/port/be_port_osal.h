/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*文件功能说明:
  本文件定义了OS抽象层OS Abstract Layer,用于统一不同的OS调用接口
 */

#ifndef BE_PORT_OSAL_H
#define BE_PORT_OSAL_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h> /* uint32_t uint8_t的类型定义 */

typedef enum {
    TimerRunOnce     = 0, /* one-shot timer */
    TimerRunPeriodic = 1  /* repeating timer */
} be_osal_timer_type;

/* task 任务优先级分配 */
#define osWaitForever 0xFFFFFFFF /* wait forever timeout value */

/* 任务优先级分配 */
#define JSE_TSK_PRIORITY be_osal_task_get_default_priority() + 5
#define CLI_TSK_PRIORITY be_osal_task_get_default_priority() + 7
#define MQTT_TSK_PRIORITY be_osal_task_get_default_priority() + 6
#define MQTTHTTP_TSK_PRIORITY be_osal_task_get_default_priority() + 4
#define SSDP_TSK_PRIORITY be_osal_task_get_default_priority() + 3
#define WEBSOCKET_TSK_PRIORITY be_osal_task_get_default_priority() + 4
#define WIFI_TSK_PRIORITY be_osal_task_get_default_priority() + 4
#define UPDATE_TSK_PRIORITY be_osal_task_get_default_priority() + 6
#define ADDON_TSK_PRIORRITY be_osal_task_get_default_priority() + 3

typedef void *osTimerId;
typedef void *osMessageQId;
typedef void (*os_timer_cb)(void *argument);

/*变量定义*/
typedef void (*be_osal_task_handler_t)(void *data);
typedef void (*be_osal_action_call_t)(void *data);
typedef void (*be_osal_event_cb_t)(void *data);

/***************************************************
 * @fn 		 be_osal_start_schedule_task
 *
 * @brief     启动内核任务调度，通常在创建多个task后，
 *            需要启动调度程序运行task，
 *            常见写法如下：main->usrcode->create task->schedule task->while(1);
 *
 * @param     无
 *
 * @return 	 无.
 **************************************************/

int32_t be_osal_start_schedule_task();

/**
 *
 * 获取默认任务优先级
 *
 */
uint32_t be_osal_task_get_default_priority(void);

/***************************************************
 * @fn			be_osal_create_task
 *
 * @brief		创建一个任务task
 *
 * @param		name：任务名称
 *              task: 任务回调函数
 *              data：传给handler的指针
 *              size：任务栈大小
 *              priority: 任务优先级，数值越大优先级越高
 *              task_handle: 用于保存task handle的二维指针，
 *              如不需在另一个task显示的销毁该任务，可以传入NULL
 *
 * @return		成功 0 失败 -1
 **************************************************/
int32_t be_osal_create_task(const char *name, be_osal_task_handler_t task,
                            void *data, uint32_t size, uint32_t priority,
                            void **task_handle);

/***************************************************
 * @fn		  be_osal_exit_task
 *
 * @brief	  退出task
 *
 * @param handle：be_osal_create_task创建task时传入task_handle得到的handle
 *			  如在本task任务中退出，可传入NULL
 *
 * @return	  无.
 **************************************************/
void be_osal_delete_task(void *handle);

/***************************************************
 * @fn 		 be_osal_get_taskname
 *
 * @brief     获取任务task名称
 *
 * @param	 无
 *
 *
 * @return 	 成功：返回该buffer的指针，失败返回null
 **************************************************/
char *be_osal_get_taskname();

/***************************************************/
/*TODO：实现task优先级接口*/
#if 0 
void be_osal_get_task_priority();
void be_osal_set_task_priority();
#endif
/***************************************************/

/***************************************************
 * @fn			be_osal_get_clocktime
 *
 * @brief		获取当前系统运行的时钟时间
 *
 * @param		无
 *
 * @return		字符串.
 **************************************************/
uint32_t be_osal_get_clocktime();

/***************************************************
 * @fn 		 be_osal_new_mutex
 *
 * @brief     初始化一个互斥信号量
 *
 * @param     无
 *
 * @return 	 void指针
 **************************************************/
void *be_osal_new_mutex();

/***************************************************
 * @fn 		 be_osal_del_mutex
 *
 * @brief     删除一个互斥信号量
 *
 * @param     be_osal_new_mutex返回的mutex指针
 *
 * @return 	 成功返回0，失败返回其他
 **************************************************/
int32_t be_osal_del_mutex(void *mutex);

/***************************************************
 * @fn 		 be_osal_lock_mutex
 *
 * @brief     获取mutex互斥锁，获取不到时会被lock住
 *
 * @param     mutex：互斥锁
 *　　　　　       ms: 超时时间，在ｍｓ时间内获取不到则返回
 *
 * @return 	 无.
 **************************************************/
int32_t be_osal_lock_mutex(void *mutex, uint32_t ms);

/***************************************************
 * @fn 		 be_osal_unlock_mutex
 *
 * @brief     释放mutex互斥锁
 *
 * @param      mutex：互斥锁指针
 *
 * @return 	 无.
 **************************************************/

int32_t be_osal_unlock_mutex(void *mutex);

/***************************************************
 * @fn 		 be_osal_delay
 *
 * @brief     延时函数，在task中延时ms毫秒
 *
 * @param     延时时间　毫秒
 *
 * @return 	 成功返回0 不支持返回-2
 **************************************************/
int32_t be_osal_delay(uint32_t ms);

/**
 * 延时10微秒
 */
void be_osal_delay10us(void);

/***************************************************
 * @fn 		 be_osal_timer_create
 *
 * @brief     创建一个定时器
 *
 * @param     callback：定时器回调函数
 *            type：定时器类型，分为单次定时器和周期定时器
 *            arg：传入回调函数的参数指针
 *
 * @return 	 定时器ID
 **************************************************/

osTimerId be_osal_timer_create(os_timer_cb callback, be_osal_timer_type type,
                               void *arg);

/***************************************************
 * @fn		 be_osal_timer_start
 *
 * @brief	 开启或者重启一个定时器
 *
 * @param	 osTimerId：定时器Id
 *			 millisec：定时器的定时时间
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_start(osTimerId timerId, uint32_t millisec);

/***************************************************
 * @fn		 be_osal_timer_stop
 *
 * @brief	 停止一个定时器
 *
 * @param	 osTimerId：定时器Id
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_stop(osTimerId timerId);

/***************************************************
 * @fn		 be_osal_timer_delete
 *
 * @brief	 删除一个定时器
 *
 * @param	 osTimerId：定时器Id
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/

int32_t be_osal_timer_delete(osTimerId timerId);

/**************************************************
 * @fn 		 be_osal_get_timer_param
 *
 * @brief     在timer回调函数中获取be_osal_timer_create的arg参数
 *
 * @param     timerHandle定时器回调函数的handle参数
 *
 *
 * @return 	 成功返回一个指针指向arg参数
 **************************************************/

void *be_osal_get_timer_param(void *timerHandle);

/**************************************************
 * @fn 		 be_osal_messageQ_create
 *
 * @brief     创建一个消息队列
 *
 * @param     queue_length: 消息对列的长度
 *            item_size: 消息队列成员的大小
 *
 * @return 	 成功返回消息对列的handler指针　失败返回NULL
 **************************************************/
osMessageQId be_osal_messageQ_create(int32_t queue_length, int32_t item_size);

/***************************************************
 * @fn		 be_osal_messageQ_delete
 *
 * @brief	 删除一个消息队列
 *
 * @param	 queue_id: 消息对列的handler ID
 *
 * @return	 成功返回0，失败返回-1
 **************************************************/
int32_t be_osal_messageQ_delete(osMessageQId queue_id);

/**************************************************
 * @fn 		 be_osal_messageQ_put
 *
 * @brief     往消息队列里面发送消息
 *
 * @param     queue_id: 消息对列的handler ID
 *            p_info: 待发送的消息buffer指针
 *            millisec: 如果消息队列满时的等待时间ms
 *
 * @return 	 发送成功返回0 失败返回其他
 **************************************************/
int32_t be_osal_messageQ_put(osMessageQId queue_id, void *p_info,
                             uint32_t millisec);

/**************************************************
 * @fn 		 be_osal_messageQ_get
 *
 * @brief     从指定的消息队列中读取消息
 *
 * @param     queue_id: 消息对列的handler ID
 *            p_info: 待发送的消息buffer指针
 *            millisec: 如果消息队列为空时的等待时间ms
 *
 * @return 	 成功返回0　失败返回-1
 **************************************************/
int32_t be_osal_messageQ_get(osMessageQId queue_id, void *p_info,
                             uint32_t millisec);

#if defined(__cplusplus)
}
#endif

#endif /* BE_PORT_OSAL_H */
