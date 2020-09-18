/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGWakeupEvent.h
 * @brief wakeup event definition
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_WAKEUP_EVENT_H
#define AG_WAKEUP_EVENT_H

/**
 * \brief wakeup event type
 */
typedef enum {
    AG_WW_EVENT_INVALID = 0,
    AG_WW_EVENT_LOCAL,
    AG_WW_EVENT_CLOUD,
    AG_WW_EVENT_VOICE_PRINT,
    AG_WW_EVENT_VOICE_MSG,
    AG_WW_EVENT_CONTINUE
} AGWakeupEvent;

#endif
