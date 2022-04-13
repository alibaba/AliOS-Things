/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file:    AGAudioRecordImpl.h
 * @brief:
 * @author:  "gaopan.rgp@alibaba-inc.com"
 * @date:    2019/10/1
 * @version: 1.0
 */

#ifndef AG_AUDIO_RECORD_IMPL1_H
#define AG_AUDIO_RECORD_IMPL1_H

#include "AGAudioRecord.h"

class AGAudioRecordImpl1 : public AGAudioRecord
{
public:
    AGAudioRecordImpl1();
    ~AGAudioRecordImpl1();

    int32_t init() override;
    int32_t getFrameSizeOfAllChannels() override;
    int32_t getFrameSizeOfOneChannel() override;
    int32_t getAudioDataFormat(int32_t* pSampleBytes,  int32_t* pSampleRate,
            int32_t* pChannelCnt) override;
    int32_t start() override;
    int32_t read(uint8_t **dataBuf) override;
    int32_t read(uint8_t **dataBuf, uint8_t chMap) override;
    int32_t read(uint8_t *buf, int bufLen, int micIndex) override;
    int32_t stop() override;
    int32_t uninit() override;
    int32_t getAudioRecordParams(AudioRecordParameters *params);

private:
    void *handle;
    int mFrames;
    uint8_t *dataBuf;
    uint8_t *_buf;
};

#endif /* ifndef AG_AUDIO_RECORD_IMPL1_H */
