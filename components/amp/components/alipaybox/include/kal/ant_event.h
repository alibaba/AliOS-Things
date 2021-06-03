/**
 * ant_event.h
 *
 * 系统事件接口
 */
#ifndef __ANT_EVENT_H__
#define __ANT_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ant_typedef.h"
#include "ant_os.h"

#define ANT_MSG_ID_IDX_BASE 0
/**
 * 事件定义
 */
typedef enum {
    ANT_EVENT_POWERON = ANT_MSG_ID_IDX_BASE,
    ANT_EVENT_SIMCARD_READY,                  // 1
    ANT_EVENT_KEY,                            // 2
    ANT_EVENT_KEY_FUNC_LONG_PRESSED,          // 3
    ANT_EVENT_KEY_COMB_FUNC_LONG_PRESSED,     // 4
    ANT_EVENT_AUDIO_PLAY_REQUEST,             // 5
    ANT_EVENT_AUDIO_PLAY_FINISHED,            // 6
    ANT_EVENT_NETWORK,                        // 7
    ANT_EVENT_NETWORK_SERVICE,                // 8  
    ANT_EVENT_NVRAM_REQ,                      // 9
    ANT_EVENT_NVRAM_RSP,                      // 10
    ANT_EVENT_DOWNLOAD_AUDIO,                 // 11
    ANT_EVENT_DOWNLOAD_ORDER,                 // 12
    ANT_EVENT_TIMESYNC,                       // 13
    ANT_EVENT_TIMESYNC_FINISH,                // 14
    ANT_EVENT_OTA,                            // 15
    ANT_EVENT_STAT_CHECK,                     // 16
    ANT_EVENT_IOT_SDK_INIT_REQ,               // 17
    ANT_EVENT_IOT_SDK_EVENT,                  // 18
    ANT_EVENT_ALIPAY_IOT_EVENT,               // 19
    // battery&charger management
    ANT_EVENT_BATTERY_CHARGE_IN,              // 20
    ANT_EVENT_BATTERY_CHARGE_OUT,             // 21
    ANT_EVENT_BATTERY_CHARGE_COMPLETE,        // 22
    ANT_EVENT_BATTERY_CHARGE_LOW_CURRENT,     // 23
    ANT_EVENT_BATTERY_CHARGE_OVER_CURRENT,    // 24
    ANT_EVENT_BATTERY_CHARGE_LOW_TEMPERATURE, // 25
    ANT_EVENT_BATTERY_CHARGE_OVER_TEMPERAURE, // 26
    ANT_EVENT_BATTERY_CHARGE_OVER_VOLTAGE,    // 27
    ANT_EVENT_BATTERY_CHARGE_STATUS,          // 28
    ANT_EVENT_USB_INSERT_STATUS,			  // 29
    // exception event
    ANT_EVENT_FS_CRASH,                       // 29
    ANT_EVENT_MEM_FAIL,                       // 30
    ANT_EVENT_MOS,                            // 31
    ANT_EVENT_LED_BLINK,                      // 32
    ANT_EVENT_FACTORY_RESET,                 // 33
    ANT_EVENT_BREAK_OFF_SHUTDOWN,             // 34
    ANT_EVENT_IDLE_EVENT,
    ANT_EVENT_VOICE_PAC_CLEAN,
    ANT_EVENT_END
} ant_event_e;


/**
 * 按键键值枚举
 */
typedef enum {
    ANT_KEY_POWER  = 0,
    ANT_KEY_FUNC,
    ANT_KEY_VOLDN,
    ANT_KEY_VOLUP,
    ANT_KEY_RESET,
    ANT_KEY_UNDEF
} ant_keycode_e;

/**
 * 按键类型枚举
 */
typedef enum {
    ANT_KEYTYPE_PRESS       = 0,
    ANT_KEYTYPE_RELEASE,
    ANT_KEYTYPE_LONG_PRESS,
    ANT_KEYTYPE_REPEAT,
    ANT_KEYTYPE_UNDEF
} ant_keytype_e;

/**
 * 按键结构体定义
 */
typedef struct {
    ant_keycode_e code;
    ant_keytype_e type;
} ant_key_event;


typedef struct{
    ant_u32 start;
    ant_u32 cnt;
    ant_u32 mode;
}ant_box_idle_t;

/**
 * 消息事件结构体定义
 */
typedef struct {
    ant_u32 message;         // ant message id
    ant_ptr param1;          // user defined value
    ant_ptr param2;          // user defined value
    ant_ptr param3;          // user defined value
    ant_ptr param4;          // user defined value
    ant_task_t src_task_id;  // task which send message
} ant_msg;

ant_bool ant_event_init(void);

/**
 * 订阅事件
 *
 * @param msg_id 订阅事件，取值参见ant_event_e
 * @return  0： 订阅成功
 *         -1： 订阅失败
 */
ant_s32 ant_event_subscribe(ant_u32 msg_id);

/**
 * 取消订阅事件
 *
 * @param msg_id 订阅事件，取值参见ant_event_e
 * @return  0： 取消订阅成功
 *         -1： 取消订阅失败
 */
ant_s32 ant_event_unsubscribe(ant_u32 msg_id);

/**
 * 广播事件。订阅了事件id的模块（任务）都会接收到该事件
 *
 * @param msg 广播事件对象
 */
void    ant_event_publish(ant_msg *msg);

/**
 * 接收事件。接收到事件之前一直阻塞。
 *
 * @param msg 返回接收到的事件
 * @return  0： 接收事件成功
 *         -1： 接收事件失败
 */
ant_s32 ant_event_recv_msg(ant_msg *msg);

/**
 * 向指定任务发送事件
 * 
 * @param dst_task_id 任务ID（即ant_task_t)
 * @param msg 发送事件的指针
 * @return  0： 发送事件成功
 *         -1： 发送事件失败
 */
ant_s32 ant_event_send_msg(ant_u32 dst_task_id, ant_msg *msg);

ant_s32 ant_event_from_system(ant_u32 msg_id);
void    ant_event_wait_bootup_completed(void);
const char *ant_event_description(ant_event_e event);

#ifdef __cplusplus
}
#endif

#endif

