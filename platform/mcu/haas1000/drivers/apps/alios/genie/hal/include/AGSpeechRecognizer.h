/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGSpeechRecognizer.h
 * @brief automatic speech recognize interface
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_SPEECH_RECOGNIZER_H
#define AG_SPEECH_RECOGNIZER_H
#include <stdint.h>
#include <stddef.h>
#include "AGWakeupWord.h"
#include "AGWakeupEvent.h"
#include "AGEOUDetectionDef.h"
enum AsrStatus {
    ASR_STATUS_START = 0,
    ASR_STATUS_VOICE,
    ASR_STATUS_SILENCE,
    ASR_STATUS_STOP,
    ASR_STATUS_FINISH
};

typedef struct ASRResult
{
    bool fgLast;
    const char* pszTempText;
    const char* pszText;
} ASRResult_T;

class AGSpeechRecognizerListener
{
public:
    virtual ~AGSpeechRecognizerListener() {}
    /**
     * @brief Callback when VAD status changes
     * @param[in] recordId
     * @param[in] vadStatus
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onVADStatusChanged(const char* recordId, int32_t vadStatus)=0;
    /**
     * @brief Callback when RMS changes
     * @param[in] recordId
     * @param[in] volume
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onRMSChanged(const char* recordId, float volume)=0;
    /**
     * @brief [Deprecated] Callback when asr result comes
     * @param[in] recordId
     * @param[in] asrResult
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onResult(const char* recordId, ASRResult_T* asrResult)=0;
    /**
     * @brief Callback when error occurs
     * @param[in] recordId
     * @param[in] error Error number
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onError(const char* recordId, int32_t error)=0;
    /**
     * @brief Callback when error occurs
     * @param[in] recordId
     * @param[in] info Information type
     * @param[in] arg1 Other parameter
     * @param[in] extInfo External information
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onInfo(const char* recordId, int32_t info, int32_t arg1, AGAIExtInfo_T* extInfo=NULL)=0;
    /**
     * @brief Callback when audio data comes
     * @param[in] id
     * @param[in] type Audio data type
     * @param[in] message
     * @param[in] dataLen Data length
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onAudioData(const char *id, int type, const void *message, int32_t dataLen)=0;
};

class AGSpeechRecognizer
{
public:
    AGSpeechRecognizer()
    {
        isReadyRun = true;
    }
    virtual ~AGSpeechRecognizer() {}
    /**
     * @brief Init AGSpeechRecognizer
     * @param[in] pSpeechRecognizerListener Register listener for callback
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t init(AGSpeechRecognizerListener* pSpeechRecognizerListener)=0;
    /**
     * @brief Start AGSpeechRecognizer
     * @param[in] config Config to set
     * @param[in] isNearASR
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t start(const AGSpeechRecognizerConfig* config, bool isNearASR=false)=0;
    /**
     * @brief
     * @param[in] fTime Finish time to set
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t setRemindFinishTime(int32_t fTime) {return 0;}
    /**
     * @brief Reset AGSpeechRecognizer
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t reset()=0;
    /**
     * @brief Feed data to do ASR
     * @param[in] data Audio data in pcm format to feed
     * @param[in] dataLen Audio data lenght to feed
     * @return int32_t Actual feed data length
     */
    virtual int32_t feed(const void *data, int32_t dataLen)=0;
    /**
     * @brief Stop AGSpeechRecognizer
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stop()=0;
    /**
     * @brief For deinit job
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t deinit()=0;
    /**
     * @brief Finish AGSpeechRecognizer
     * @return VOID
     */
    virtual void finish(void)
    {
        isReadyRun = true;
    }

    /**
     * @brief Get status for AGSpeechRecognizer
     * @return true or false
     */
    virtual bool isReady(void)
    {
        return isReadyRun;
    }

    /**
     * @brief Get running status for AGSpeechRecognizer
     * @return true or false
     */
    virtual void onRunning(void)
    {
        isReadyRun = false;
    }

private:
    bool isReadyRun;

};
#endif
