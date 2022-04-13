/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGVad.h
 * @brief automatic speech recognize interface
 * @author tanyan.lk@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_VAD_H
#define AG_VAD_H

class AGVadListener
{
public:
    virtual ~AGVadListener() {}
    /**
     * @brief Callback when VAD status changes
     * @param[in] recordId
     * @param[in] vadStatus
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onVADStatusChanged(int32_t vadStatus)=0;
};

class AGVad
{
public:
    AGVad() {}
    virtual ~AGVad() {}
    /**
     * @brief Init AGVad
     * @param[in] pSpeechRecognizerListener Register listener for callback
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t init(AGVadListener* pVadListener)=0;
    /**
     * @brief Start AGVad
     * @param[in] config Config to set
     * @param[in] isNearASR
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t start()=0;
    /**
     * @brief Feed data to do ASR
     * @param[in] data Audio data in pcm format to feed
     * @param[in] dataLen Audio data lenght to feed
     * @return int32_t Actual feed data length
     */
    virtual int32_t feed(const void *data, int32_t dataLen)=0;
    /**
     * @brief Stop AGVad
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stop()=0;
    /**
     * @brief For deinit job
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t deinit()=0;
};
#endif
