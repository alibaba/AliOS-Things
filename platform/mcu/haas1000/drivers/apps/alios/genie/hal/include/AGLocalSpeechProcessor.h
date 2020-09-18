/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGLocalSpeechProcessor.h
 * @brief local speech engine interface
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_AUDIO_SIGNAL_PROCESSOR_AND_WAKEUP_WORD_DETECTOR_H
#define AG_AUDIO_SIGNAL_PROCESSOR_AND_WAKEUP_WORD_DETECTOR_H

#include <stdint.h>
#include "AGWakeupWord.h"
#include "AGEOUDetectionDef.h"
#include "AGLocalASRDef.h"

/**
 * \brief type of the audio data from signal processor.
 */
enum AudioDataStatus
{
    AudioNormal=0,      /**< normal type, if you don't known rollback feature, just use this one. */
    AudioRollbackStart, /**< rollback type */
    AudioRollback,      /**< rollback type */
    AudioRollbackEnd    /**< rollback type */
};

/**
 * \brief rollback feature
 */
enum AGASPFeature
{
    AG_ASP_FEATURE_ROLLBACK = 1 << 0
};

/**
 * \brief event type of the wakeup
 * \deprecated use wakeup word name
 */
enum WakeupEvent {
    WAKEUP_EVENT_INVALID = -1,
    WAKEUP_EVENT_SILENCE = 0,
    WAKEUP_EVENT_ACTIVE,
    WAKEUP_EVENT_CONTINUE,
    WAKEUP_EVENT_CONTINUE_DIAGLOG,
    WAKEUP_EVENT_CLOUD,
    WAKEUP_EVENT_VOICE_PRINT,
    WAKEUP_EVENT_VOICE_MSG,
    WAKEUP_EVENT_PAUSE,
    WAKEUP_EVENT_RESUME,
    WAKEUP_EVENT_EXIT,
    WAKEUP_EVENT_SHUTDOWN,
    WAKEUP_EVENT_STOP,
    WAKEUP_EVENT_PLAY,
    WAKEUP_EVENT_NEXT,
    WAKEUP_EVENT_NEXT_SONG,
    WAKEUP_EVENT_PREV,
    WAKEUP_EVENT_VOLUME_INC,
    WAKEUP_EVENT_VOLUME_DEC,
    WAKEUP_EVENT_KEY,
    WAKEUP_EVENT_TOUCH,
    WAKEUP_EVENT_APP,
    WAKEUP_EVENT_CLOUD_SHORTCUT = 0x1 <<10,
    WAKEUP_NEGATIVE_CASE = 0x1<<11,
    WAKEUP_EVENT_DYNAMIC_WAKEUP = 0x1 << 12
};

/**
 * \brief wakeup scence, you adjust thresh by this info
 */
enum WakeupScence {
    WAKEUP_SCENCE_DAY = 0,
    WAKEUP_SCENCE_NIGHT
};

/**
 * \brief inform infomation to smartbox use this listener
 */
class AGLocalSpeechProcessorListener
{
public:
    virtual ~AGLocalSpeechProcessorListener() {}
    /**
     * @brief Pass the processed audio data to smartbox
     * @param[in] data pointer to audio data, only support 16bit/16khz/mono pcm
     * @param[in] dataLen len of audio data in bytes
     * @param[in] status type of the audio data from signal processor, use AudioNormal if don't known rollback
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onAudioData(const void *data, int32_t dataLen, AudioDataStatus status)=0;
    /** \deprecated */
    virtual int32_t onWakeupEvent(float confidence, int32_t wakeupEvent, const char *wakeupWord)=0;
    /**
     * @brief Pass wakeup info to smartbox
     * @param[in] confidence Confidence of the wakeup detection
     * @param[in] wakewordName Name of wakeup word
     * @param[in] wakeupWord Phonetic of wakeup word
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onWakeupEvent(float confidence, AGWakeupWordName wakewordName, const char *wakeupWord)=0;
    /**
     * @brief Pass wakeup info to smartbox
     * @param[in] confidence Confidence of the wakeup detection
     * @param[in] wakewordName Name of wakeup word
     * @param[in] wakeupWord Phonetic of wakeup word
     * @param[in] chnWakeupWord Chinese characters of wakeup word, encoding in UTF-8
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onWakeupEvent(float confidence,
                                        AGWakeupWordName wakewordName,
                                        const char *wakeupWord,
                                        const char *chnWakeupWord,
                                        bool needASRVerify){return 0;}
    /**
     * @brief Doa event callback, doa to smartbox
     * @param[in] doa Doa value of the wakeup
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onDoaEvent(int32_t doa) = 0;
    /**
     * @brief Local asr result callback
     * @param[in] pASRResult ASR result in string
     * @param[in] fgFinal Whether it's final ASR
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t onLocalASRResult(AGLocalASRResult_T* pASRResult) = 0;
    /** \deprecated */
    virtual int32_t onLocalASRInfo(
        const char* recordId, int32_t info, int32_t arg1, AGAIExtInfo_T* extInfo){return 0;}
};

/**
 * \brief audio signal processor and wakeup detection interface
 */
class AGLocalSpeechProcessor
{
public:
    virtual ~AGLocalSpeechProcessor() {}
    /**
     * @brief Determine if some feature is supported
     * @param[in] feature feature to be determined
     * @return  ture if support, false otherwise
     */
    virtual bool    hasFeature(AGASPFeature feature);
    /**
     * @brief Init AGLocalSpeechProcessor with listener
     * @param[in] pLspListener Listener to register
     * @param[in] wakeupScence Wake up scence type
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t init(AGLocalSpeechProcessorListener* pLspListener, WakeupScence wakeupScence)=0;
    /**
     * @brief Get the processed audio data format,only 16bit/16khz/mono for now
     * @param[in] pSampleBytes Bytes per sample
     * @param[in] pSampleRate Audio pcm sample rate
     * @param[in] pChannelCnt Audio pcm channel count
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t getAudioDataFormat(
        int32_t* pSampleBytes,  int32_t* pSampleRate, int32_t* pChannelCnt)=0;
    /**
     * @brief Set the wakeup scence to use
     * @param[in] wakeupScence Wake up scence type
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t setWakeupScence(WakeupScence wakeupScence)=0;
    /**
     * @brief Update wakup words
     * @param[in] type Which type of wake up word to update
     * @param[in] wwords Wake up words array
     * @param[in] sz Size of wake up words array
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t useWakeupWords(AGWakeupWordType type, void* wwords, int32_t sz) {return 0;}
    /**
     * @brief Called when system volume change
     * @param[in] index The current volume, [0,100]
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t handleVolumeChange(int32_t index) {return 0;}
    /**
     * @brief Start process audio data
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t start()=0;
    /**
     * @brief Feed audio pcm data
     * @param[in] data Audio data in pcm format to feed
     * @param[in] dataLen Audio data lenght to feed
     * @return int32_t Actual feed data length
     */
    virtual int32_t feed(const void *data, int32_t dataLen)=0;
    /**
     * @brief For future use
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t pause() = 0;
    /**
     * @brief For future use
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t resume() = 0;
    /**
     * @brief Stop process data
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stop()=0;
    /**
     * @brief Start local asr
     * @param[in] config Speech recognizer config
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t startLocalASR(const AGSpeechRecognizerConfig* config){return 0;}
    /**
     * @brief Start local asr, no need for eou
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t startLocalASR(){return 0;}
    /**
     * @brief Feed audio pcm data
     * @param[in] data Audio data in pcm format to feed
     * @param[in] dataLen Audio data lenght to feed
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t feedLocalASR(const void *data, int32_t dataLen){return 0;}
    /**
     * @brief Feed string to get nlu result
     * @param[in] data string to understand
     * @param[in] dataLen string len
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t feedASRData(const void *data, int32_t dataLen){return 0;}
    /**
     * @brief Stop local asr
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stopLocalASR(){return 0;}
    /**
     * @brief Handle result from cloud asr
     * @param[in] pASRResult Cloud asr result in string
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t handleCloudASRResult(const char* pASRResult){return 0;}
    /**
     * @brief To check if we should wakeup
     * @param[in] pChnWakeupWord Chinese wakeup word in string
     * @param[in] pASRResult Local asr result in string
     * @return bool true : success; other: false
     */
    virtual bool isWakeupValid(const char* pChnWakeupWord,const char* pASRResult){return 0;}
    /**
     * @brief Deinit local asr
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t deinit()=0;
};

#endif
