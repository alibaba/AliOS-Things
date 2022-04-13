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

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TG_SPEECH_USE_AG
#if 0
#define TG_SPEECH_KEY_TRIGGER
#endif

#ifdef TG_SPEECH_USE_AG
enum VAD_EVT {
    AG_VAD_STOP = 0,
    AG_VAD_START = 1,
    AG_VAD_VOICE = 2,
    AG_VAD_SILENCE = 3
};

typedef void (*ag_wakeup_cb)(void);
typedef void (*ag_vad_cb)(enum VAD_EVT evt);
typedef void (*ag_audio_ind_cb)(unsigned char *data, int data_len);
typedef struct _AG_RECORD_CB_ {
    ag_wakeup_cb wakeup_cb;
    ag_vad_cb vad_cb;
    ag_audio_ind_cb audio_ind_cb;
}AG_RECORD_CB;

int ag_audio_record_force_start(void);
int ag_audio_record_get_len();
int ag_audio_record_init(AG_RECORD_CB * callback);
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
    ENGINE_ROLLBACK_END, //唤醒词 rollback 区块已读空
    ENGINE_MUSIC_VOICE = 15,
    ENGINE_EOU_NOTIFY,
    ENGINE_FULLDIALOG_NOTIFY
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
void set_wakeup_response_flag(bool wakeup_response);
#ifdef __cplusplus
}
#endif

#endif /* _TG_SPEECH_H_ */
