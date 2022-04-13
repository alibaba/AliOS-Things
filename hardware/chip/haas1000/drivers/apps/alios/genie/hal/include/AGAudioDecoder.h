/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGAudioDecoder.h
 * @brief audio decoder 
 * @author wangxiaowei.wxw@alibaba-inc.com
 * @data 2019/07/06
 * @version 1.0
 */

#ifndef AG_AUDIO_DECODER_H
#define AG_AUDIO_DECODER_H

#include <stdint.h>

typedef enum
{
    AG_AUDIO_DECODER_FORMAT_INVALID= -1,
    AG_AUDIO_DECODER_FORMAT_MP3 = 0,
    AG_AUDIO_DECODER_FORMAT_AAC,
    AG_AUDIO_DECODER_FORMAT_OPUS
} AGAudioDecoderFormat;


class AGAudioDecoder
{
public:
    virtual ~AGAudioDecoder(){}
    /**
     * @brief Start audio decoder
     * @param[in] AGAudioDecoderFormat     audio format
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t open(AGAudioDecoderFormat format);
    /**
     * @brief Send audio stream per frame to decoder and read pcm data back
     * @param[in]       data_in       audio frame buffer
     * @param[in]       input_size    length of input audio frame
     * @param[inout]    data_out      audio pcm buffer
     * @param[inout]    output_size   length of output pcm buffer
     * @param[out]      output_sample length of output sample

     * @return int32_t greate than or equal to zero : success; less than zero: fail
     */
    virtual int32_t decoder(uint8_t *data_in, int input_size, uint8_t *data_out, int *output_size, int *output_sample, int *sample_rate);
    /**
     * @brief Stop audio track, geniesdk will call start to make it active again
     *         stop will cause more to make it active to pause
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t close();

};

#endif

