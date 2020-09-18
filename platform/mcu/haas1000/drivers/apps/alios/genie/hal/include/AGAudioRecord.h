/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGAudioRecord.h
 * @brief Audio record api
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_AUDIO_RECORD_H
#define AG_AUDIO_RECORD_H

#include "stdint.h"

#undef read

/***Extra audio parameters for audio input settings.
       *channel_count: Number of input channels.
       *ref_channel_index: Channel index for reference audio data.
       */
typedef struct _AudioRecordParameters
{
    int channel_count;
    int ref_channel_index;
} AudioRecordParameters;


class AGAudioRecord
{
public:
    virtual ~AGAudioRecord(){}
    /**
     * @brief Create audio record
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t init()=0;
    /**
     * @brief Called after init to get the frame size of all channels, in bytes
     * @return int32_t Frame size in bytes
     */
    virtual int32_t getFrameSizeOfAllChannels()=0;
    /**
     * @brief Called after init to get the frame size of one channel, in bytes
     * @return int32_t Frame size in bytes
     */
    virtual int32_t getFrameSizeOfOneChannel()=0;
    /**
     * @brief Get audio format, for now only 2bytes,16000hz supported
     * @param[in] pSampleBytes Bytes per sample
     * @param[in] pSampleRate Audio pcm sample rate
     * @param[in] pChannelCnt Audio pcm channel count
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t getAudioDataFormat(int32_t* pSampleBytes,  int32_t* pSampleRate, int32_t* pChannelCnt)=0;
    /**
     * @brief Start audio record
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t start()=0;
    /**
     * @brief Read all mics, return buffer pointer by dataBuf.
     * @param[in] dataBuf Buffer to store data
     * dataBuf's size must equal to return valude of getFrameSizeOfAllChannels Bytes per sample 
     * @return int32_t Actual read size.
     */
    virtual int32_t read(uint8_t **dataBuf)=0;
    /**
     * @brief Read all mics, return buffer pointer by dataBuf.
     * @param[in] dataBuf Buffer to store data
     * dataBuf's size must equal to return valude of getFrameSizeOfAllChannels Bytes per sample
     * @param[in] resume indicate the status for mic
     * @return int32_t Actual read size.
     */
    virtual int32_t read(uint8_t **dataBuf, uint8_t resume)=0;
    /**
     * @brief Read one mic, only required for wifi config
     * @param[in] buf Buffer to store data
     * @param[in] bufLen Buffer length
     * @param[in] micIndex Min index to read
     * @return int32_t Actual read size.
     */
    virtual int32_t read(uint8_t *buf, int bufLen, int micIndex)=0;
    /**
     * @brief Stop record, geniesdk will call start to make it active again
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stop()=0;
    /**
     * @brief Delete audio track
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t uninit()=0;
    virtual int32_t getAudioRecordParams(AudioRecordParameters *params)=0;
};

#endif

