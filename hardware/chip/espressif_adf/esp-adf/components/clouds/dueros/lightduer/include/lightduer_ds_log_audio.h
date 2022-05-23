/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Auth: Chen Xihao(chenxihao@baidu.com)
 * Desc: audio play related report log
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_AUDIO_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_AUDIO_H

#include "baidu_json.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_ds_log_audio_code {
    DUER_DS_LOG_AUDIO_BUFFER_INFO               = 0x001,
    DUER_DS_LOG_AUDIO_PLAY_START                = 0x002,
    DUER_DS_LOG_AUDIO_PLAY_PAUSE                = 0x003,
    DUER_DS_LOG_AUDIO_PLAY_RESUME               = 0x004,
    DUER_DS_LOG_AUDIO_PLAY_STOP                 = 0x005,
    DUER_DS_LOG_AUDIO_PLAY_FINISH               = 0x006,
    DUER_DS_LOG_AUDIO_CODEC_VERSION             = 0x007,
    DUER_DS_LOG_AUDIO_INFO                      = 0x008,
    DUER_DS_LOG_AUDIO_M4A_HEAD_SIZE             = 0x009,
    DUER_DS_LOG_AUDIO_CONTENT_TYPE              = 0x00a,
    DUER_DS_LOG_AUDIO_DOWNLOAD_DELAY            = 0x00b,
    DUER_DS_LOG_AUDIO_PLAY_DELAY                = 0x00c,

    DUER_DS_LOG_AUDIO_NO_MEMORY                 = 0x301,
    DUER_DS_LOG_AUDIO_UNSUPPORT_FORMAT          = 0x302,
    DUER_DS_LOG_AUDIO_UNSUPPORT_BITRATE         = 0x303,
    DUER_DS_LOG_AUDIO_HEAD_TOO_LARGE            = 0x304,
    DUER_DS_LOG_AUDIO_DOWNLOAD_FAILED           = 0x305,
    DUER_DS_LOG_AUDIO_PLAYER_ERROR              = 0x306,
    DUER_DS_LOG_AUDIO_BUFFER_ERROR              = 0x307,
    DUER_DS_LOG_AUDIO_CODEC_ERROR               = 0x308,
    DUER_DS_LOG_AUDIO_NULL_POINTER              = 0x309,
    DUER_DS_LOG_AUDIO_INVALID_CONTEXT           = 0x30a,
    DUER_DS_LOG_AUDIO_URL_NO_DATA               = 0x30b,
    DUER_DS_LOG_AUDIO_PLAY_FAILED               = 0x30c,

    DUER_DS_LOG_AUDIO_M4A_INVALID_HEADER        = 0x401,
    DUER_DS_LOG_AUDIO_M4A_PARSE_HEADER_ERROR    = 0x402,

    DUER_DS_LOG_AUDIO_HLS_PARSE_M3U8_ERROR      = 0x501,
    DUER_DS_LOG_AUDIO_HLS_DOWNLOAD_ERROR        = 0x502,
    DUER_DS_LOG_AUDIO_HLS_RELOAD_ERROR          = 0x503,

    DUER_DS_LOG_AUDIO_SOFT_DECODER_ERROR        = 0x601,

    DUER_DS_LOG_AUDIO_USER_DEFINED_ERROR        = 0xf01,
} duer_ds_log_audio_code_t;

typedef struct duer_ds_audio_info_s {
    duer_u32_t duration;
    duer_u32_t block_count;
    duer_u32_t max_codec_bitrate;
    duer_u32_t min_codec_bitrate;
    duer_u32_t avg_codec_bitrate;
    duer_u32_t request_play_ts;
    duer_u32_t request_download_ts;
} duer_ds_audio_info_t;

#define DUER_DS_LOG_AUDIO_MEMORY_OVERFLOW() duer_ds_log_audio_memory_overflow(__FILE__, __LINE__)

#define DUER_DS_LOG_AUDIO_NULL_POINTER()    duer_ds_log_audio_null_pointer(__FILE__, __LINE__)

#define DUER_DS_LOG_AUDIO_INVALID_CONTEXT() duer_ds_log_audio_invalid_context(__FILE__, __LINE__)

/**
 * common function to report the log of audio play moudle
 */
duer_status_t duer_ds_log_audio(duer_ds_log_audio_code_t log_code, const baidu_json *log_message);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_BUFFER_INFO
 *  {
 *     "size": [buffer size],
 *     "type": [buffer type] // 0: heap, 1: data segment, 2: psram 3: file
 *  }
 */
duer_status_t duer_ds_log_audio_buffer_info(duer_u32_t size, duer_u32_t type);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAY_START
 *  {
 *     "url": [url of audio],
 *     "source_type": [source type]
 *  }
 */
duer_status_t duer_ds_log_audio_play_start(const char *url, duer_u32_t type);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAY_STOP
 *  {
 *     "duration": [播放时长],
 *     "block_count": [卡顿次数],
 *     "codec_bitrate": {
 *         "max" : [最大码率],
 *         "min" : [最小码率],
 *         "avg" : [平均码率]
 *     }
 *  }
 */
duer_status_t duer_ds_log_audio_play_stop(duer_u32_t duration, duer_u32_t block_count,
        duer_u32_t max_bitrate, duer_u32_t min_bitrate, duer_u32_t avg_bitrate);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAY_FINISH
 *  {
 *     "duration": [播放时长],
 *     "block_count": [卡顿次数],
 *     "codec_bitrate": {
 *         "max" : [最大码率],
 *         "min" : [最小码率],
 *         "avg" : [平均码率]
 *     }
 *  }
 */
duer_status_t duer_ds_log_audio_play_finish(duer_u32_t duration, duer_u32_t block_count,
        duer_u32_t max_bitrate, duer_u32_t min_bitrate, duer_u32_t avg_bitrate);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_CODEC_VERSION
 *  {
 *     "version": [codec版本号]
 *  }
 */
duer_status_t duer_ds_log_audio_codec_version(const char *version);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_INFO
 *  {
 *     "bitrate": [比特率],
 *     "sample_rate": [采样率],
 *     "bits_per_sample": [位宽],
 *     "channels": [声道数]
 *  }
 */
duer_status_t duer_ds_log_audio_info(duer_u32_t bitrate,
        duer_u32_t sample_rate, duer_u32_t bits_per_sample, duer_u32_t channels);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_M4A_HEAD_SIZE
 *  {
 *     "head_size": [m4a头大小]
 *  }
 */
duer_status_t duer_ds_log_audio_m4a_head_size(duer_u32_t size);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_CONTENT_TYPE
 *  {
 *     "content_type": [http下载类型]
 *  }
 */
duer_status_t duer_ds_log_audio_content_type(const char *type);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_DOWNLOAD_DELAY
 *  {
 *     "delay": [时延，单位ms]
 *  }
 */
duer_status_t duer_ds_log_audio_download_delay(void);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAY_DELAY
 *  {
 *     "delay": [时延，单位ms]
 *  }
 */
duer_status_t duer_ds_log_audio_play_delay(void);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_NO_MEMORY
 *  {
 *     "file": [发生内存分配错误的文件],
 *     "line": [发生内存分配错误的行号]
 *  }
 */
duer_status_t duer_ds_log_audio_memory_overflow(const char *file, duer_u32_t line);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_NULL_POINTER
 *  {
 *     "file": the file name where error happens
 *     "line": the line number where error happens
 *  }
 */
duer_status_t duer_ds_log_audio_null_pointer(const char *file, duer_u32_t line);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_INVALID_CONTEXT
 *  {
 *     "file": the file name where error happens
 *     "line": the line number where error happens
 *  }
 */
duer_status_t duer_ds_log_audio_invalid_context(const char *file, duer_u32_t line);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_HLS_PARSE_M3U8_ERROR
 *  {
 *     "line": [解析出错的行号]
 *  }
 */
duer_status_t duer_ds_log_audio_hls_parse_m3u8_error(duer_u32_t line);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_FRAMEWORK_ERROR
 *  {
 *     "error_code": Internal error code for audio framework
 *  }
 */
duer_status_t duer_ds_log_audio_framework_error(int error_code);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAYER_ERROR
 *  {
 *     "error_code": Internal error code for audio player
 *  }
 */
duer_status_t duer_ds_log_audio_player_error(int error_code);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_PLAY_FAILED
 *  {
 *     "url": [音频url]
 *  }
 */
duer_status_t duer_ds_log_audio_play_failed(const char *url);

/**
 * report the message for log code DUER_DS_LOG_AUDIO_USER_DEFINED_ERROR
 *  {
 *     "error_code": user defined error code
 *  }
 */
duer_status_t duer_ds_log_audio_user_defined_error(int error_code);

/**
 * record the timestamp when request to play
 */
duer_status_t duer_ds_log_audio_request_play(void);

/**
 * record the timestamp when request to download
 */
duer_status_t duer_ds_log_audio_request_download(void);

extern duer_ds_audio_info_t g_audio_infos;

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_AUDIO_H
