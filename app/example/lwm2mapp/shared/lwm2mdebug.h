/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Simon Bernard - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/ 
#ifndef LWM2MDEBUG_H_
#define LWM2MDEBUG_H_
#include "ulog/ulog.h"

#define TAG "lwm2mapp"

#ifdef LWM2M_WITH_LOGS
#include <inttypes.h>
#define lwm2m_log(level, ...) do { \
        if( level == LOG_ERR) { \
           LOGE(TAG, __VA_ARGS__); \
        } \
        else if( level == LOG_WARNING) { \
           LOGW(TAG, __VA_ARGS__); \
        } \
        else if( level == LOG_INFO) { \
           LOGI(TAG, __VA_ARGS__); \
        } \
        else if( level == LOG_DEBUG) { \
           LOGD(TAG, __VA_ARGS__); \
        } \
        } while(0)
#else
#ifndef lwm2m_log
#define lwm2m_log(level, ...) do { \
            if( level == LOG_ERR) { \
                LOGE(TAG, __VA_ARGS__); \
            } \
            else if( level == LOG_WARNING) { \
                LOGW(TAG, __VA_ARGS__); \
            } \
            else if( level == LOG_INFO) { \
                LOGI(TAG, __VA_ARGS__); \
            } \
        } while(0)
#endif
#endif
#endif /* LWM2MDEBUG_H_ */
