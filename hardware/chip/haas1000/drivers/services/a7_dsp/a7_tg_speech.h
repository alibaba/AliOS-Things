/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts_speech.h
 * @brief:
 * @author:  
 * @date:    2019/12/15
 * @version: 1.0
 */

#ifndef _TG_SPEECH_H_
#define _TG_SPEECH_H_

#ifdef __cplusplus
extern "C" {
#endif

enum AGEngineCMD {
    ENGINE_WAKEUP_SCENCE, //引擎夜间模式/白天模式的切换
    ENGINE_OPERATOR, //引擎的操作，目前为静音/收音
    ENGINE_ROLLBACK, //唤醒词检测到之后 rollback 开始/结束请求
    ENGINE_VOLUME //当前音量大小
};

typedef enum
{
    ENGINE_WAKEUP_DETECT = 1, //检测到唤醒词
    ENGINE_WAKEUP_ONLY, //预留，区分 wakeup only 和 oneshot 类型
    ENGINE_WAKEUP_ONESHOT, //预留，区分 wakeup only 和 oneshot 类型
    ENGINE_WAKEUP_ENERGY, //唤醒词能量值
    ENGINE_WAKEUP_SHORTCUT, //预留，区分快捷唤醒词
    ENGINE_VAD_DETECT, //VAD 检测
    ENGINE_VAD_NO_VOICE, //预留，Leading silence 检测
    ENGINE_VAD_VOICE_END, //预留，Tail silence 检测
    ENGINE_VOICE_DATA, //AEC 之后的 raw data
    ENGINE_ROLLBACK_END //唤醒词 rollback 区块已读空
} engine_evt_id_t;

typedef struct
{
    engine_evt_id_t evt_id; //event 类型
    void *data; //数据块在共享内存的起始地址
    int extValue; //extValue, 可变参数
    int len; // 数据块的长度
} callback_msg_t;

void tg_speech_message_receive(callback_msg_t *msg);
void tg_speech_msg_send(void* msg, int size, bool sync);
typedef void (*tg_speech_msg_callback_t)(void *msg, int size);
void tg_speech_msg_register(tg_speech_msg_callback_t cbf);
void tg_voip_msg_send(void* msg, int size, bool sync);
void tg_voip_msg_register(tg_speech_msg_callback_t cbf);

#ifdef __cplusplus
}
#endif

#endif /* _TG_SPEECH_H_ */
