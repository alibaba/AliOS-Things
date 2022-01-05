// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _WAV_HEADER_H_
#define _WAV_HEADER_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CHUNKID_RIFF (0X46464952)
#define CHUNKID_WAVE (0X45564157)
#define CHUNKID_FMT (0X20746D66)
#define CHUNKID_DATA (0X61746164)

/**
* brief     RIFF block
*/
typedef struct {
    uint32_t chunk_id;               /*!<chunk id;"RIFF",0X46464952 */
    uint32_t chunk_size;             /*!<file length - 8 */
    uint32_t format;                /*!<WAVE,0X45564157 */
} chunk_riff_t;

/**
* brief     FMT block
*/
typedef struct {
    uint32_t chunk_id;               /*!<chunk id;"fmt ",0X20746D66 */
    uint32_t chunk_size;             /*!<Size of this fmt block (Not include ID and Size);16 or 18 or 40 bytes. */
    uint16_t audio_format;           /*!<format;0X01:linear PCM;0X11:IMA ADPCM */
    uint16_t num_of_channels;         /*!<Number of channel;1: 1 channel;2: 2 channels; */
    uint32_t samplerate;            /*!<sample rate;0X1F40 = 8Khz */
    uint32_t byterate;              /*!<Byte rate; */
    uint16_t block_align;            /*!<align with byte; */
    uint16_t bits_per_sample;         /*!<Bit lenght per sample point,4 ADPCM */
    // uint16_t ByteExtraData;      /*!<Exclude in linear PCM format(0~22) */
} __attribute__((packed)) chunk_fmt_t;

/**
* brief     FACT block
*/
typedef struct {
    uint32_t chunk_id;               /*!<chunk id;"fact",0X74636166; */
    uint32_t chunk_size;             /*!<Size(Not include ID and Size);4 byte */
    uint32_t num_of_samples;          /*!<number of sample */
} __attribute__((packed)) chunk_fact_t;

/**
* brief     LIST block
*/
typedef struct {
    uint32_t chunk_id;                /*!<chunk id 0X5453494C; */
    uint32_t chunk_size;              /*!<Size */
} __attribute__((packed)) chunk_list_t;

/**
* brief     PEAK block
*/
typedef struct {
    uint32_t chunk_id;                 /*!<chunk id; 0X4B414550 */
    uint32_t chunk_size;               /*!<Size */
} __attribute__((packed)) chunk_peak_t;

/**
* brief     Data block
*/
typedef struct {
    uint32_t chunk_id;                  /*!<chunk id;"data",0X5453494C */
    uint32_t chunk_size;                /*!<Size of data block(Not include ID and Size) */
} __attribute__((packed)) chunk_data_t;

/**
* brief     subchunk block
*/
typedef struct {
    uint32_t chunk_id;                  /*!<sub chunk id */
    uint32_t chunk_size;                /*!<Size of data block(Not include ID and Size) */
} __attribute__((packed)) sub_chunk_t;

/**
* brief     WAV block
*/
typedef struct {
    chunk_riff_t riff;                    /*!<riff */
    chunk_fmt_t fmt;                      /*!<fmt */
    //  chunk_fact_t fact;                /*!<fact,Exclude in linear PCM format */
    chunk_data_t data;                    /*!<data */
} __attribute__((packed)) wav_header_t;

/**
* brief     WAV control struct
*/
typedef struct {
    uint16_t audio_format;              /*!<format;0X01,linear PCM;0X11 IMA ADPCM */
    uint16_t channels;                  /*!<Number of channel;1: 1 channel;2: 2 channels; */
    uint16_t block_align;               /*!<align */
    uint32_t data_size;                 /*!<size of data */
    uint32_t totsec;                    /*!<total time,second */
    uint32_t cursec;                    /*!<current time, second */
    uint32_t bitrate;                   /*!<bit sample rate */
    uint32_t samplerate;                /*!<sample rate */
    uint16_t bits;                      /*!<bit length 16bit,24bit,32bit */ 
    uint32_t data_shift;                /*!<Data shift. */
    uint32_t data_remain;               /*!<Data remain. */
    uint32_t head_size;                 /*!<Wav header size. */
} __attribute__((packed)) wav_info_t;

/**
* @brief      Check whether this stream is WAV or not
*
* @param      in_data          The input stream data of WAV stream
* @param      len             The length of input stream data
*
* @return
*             - ESP_OK
*             - ESP_FAIL
*/
esp_err_t wav_check_type(uint8_t *in_data, int len);

/**
* @brief      Initialize the header information of WAV stream
*
* @param      wavhead         The header information of WAV stream
* @param      sample_rate     The sample rate of WAV stream
* @param      bits            The bit width of WAV stream
* @param      channels        The number channel(s) of WAV stream
*/
void wav_head_init(wav_header_t *wavhead, int sample_rate, int bits, int channels);

/**
* @brief      Parse the header of WAV stream
*
* @param      codec_data      The handle of codec_data
* @param      inData          The input stream data of WAV stream
* @param      len             The length of input stream data
* @param      info            The header information of stream data
*
* @return
*             - ESP_OK
*             - ESP_FAIL
*/
esp_err_t wav_parse_header(void *codec_data, uint8_t *inData, int len, wav_info_t *info);

/**
* @brief      Get audio data size of WAV stream
*
* @param      wavhead         The header information of WAV stream
* @param      dataSize        The size of WAV stream
*/
void wav_head_size(wav_header_t *wavhead, uint32_t dataSize);

#ifdef __cplusplus
}
#endif

#endif
