/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGInput.h
 * @brief
 * @author chiliang.cl@alibaba-inc.com
 * @data 2018/03/09
 * @version 1.0
 */

#ifndef AG_INPUT_H
#define AG_INPUT_H

enum InputEventType {
    INPUT_EVENT_INVALID = 0,
    INPUT_EVENT_INC_VOL,
    INPUT_EVENT_DEC_VOL,
    INPUT_EVENT_WIFI_SETUP,
    INPUT_EVENT_FACTORY_RESET,
    INPUT_EVENT_MIC_MUTE,
    INPUT_EVENT_BT_PAIR,
    INPUT_EVENT_REMOTEASSISTANT,
    INPUT_EVENT_FACTORY_ST,
    INPUT_EVENT_FACTORY_OTA,
    INPUT_EVENT_BUTTON_WAKEUP,
    INPUT_EVENT_NEARVOICE_START,
    INPUT_EVENT_NEARVOICE_END,
    INPUT_EVENT_AUDIO_OUTPUT_SWITCH,
    INPUT_EVENT_MIC_MUTE_FROM_SYSTEM,
    INPUT_EVENT_MIC_UNMUTE_FROM_SYSTEM,
    INPUT_EVENT_PHONE_HANGUP
};

typedef struct
{
    int eventType;
} InputMsgData_T;

class AGInputMsgListener
{
public:
    virtual ~AGInputMsgListener() {}
    /**
     * @brief Receive input message
     * @param[in] pInputMsgData
     * @return void
     */
    virtual void onRecvInputMsg(InputMsgData_T *pInputMsgData) = 0;
};

class AGInput
{
public:
    virtual ~AGInput() {}
    /**
     * @brief Initialize AGInput
     * @param[in] pInputMsgListener
     * @return void
     */
    virtual void init(AGInputMsgListener* pInputMsgListener) = 0;
    virtual void start() = 0;
};
#endif
