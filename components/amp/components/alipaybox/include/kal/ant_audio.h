/**
 * ant_audio.h
 *
 * 音频播放接口
 */
#ifndef __ANT_AUDIO_H__
#define __ANT_AUDIO_H__
#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANT_AUDIO_MIN_VOLUME 1
#define ANT_AUDIO_MAX_VOLUME 6

typedef enum {
    ANT_AUDIO_FORMAT_UNKNOWN = -1,
    ANT_AUDIO_FORMAT_WAV = 0,
    ANT_AUDIO_FORMAT_MP3 = 1,
    ANT_AUDIO_FORMAT_BUFFER = 2
} ant_audio_format_e;

typedef enum {
    ANT_AUDIO_SUCCESS = 0,          /* 0 */
    ANT_AUDIO_FAIL,                 /* 1 */
    ANT_AUDIO_BUSY,                 /* 2 */
    ANT_AUDIO_DISC_FULL,            /* 3 */
    ANT_AUDIO_OPEN_FILE_FAIL,       /* 4 */
    ANT_AUDIO_END_OF_FILE,          /* 5 */
    ANT_AUDIO_TERMINATED,           /* 6 */
    ANT_AUDIO_BAD_FORMAT,           /* 7 */
    ANT_AUDIO_INVALID_FORMAT,       /* 8 */
    ANT_AUDIO_ERROR,                /* 9 */
    ANT_AUDIO_NO_DISC,              /* 10 */
    ANT_AUDIO_NO_SPACE,             /* 11 */
    ANT_AUDIO_INVALID_HANDLE,       /* 12 */
    ANT_AUDIO_NO_HANDLE,            /* 13 */
    ANT_AUDIO_RESUME,               /* 14 */
    ANT_AUDIO_BLOCKED,              /* 15 */
    ANT_AUDIO_MEM_INSUFFICIENT,     /* 16 */
    ANT_AUDIO_BUFFER_INSUFFICIENT,  /* 17 */
    ANT_AUDIO_FILE_EXIST,           /* 18 */
    ANT_AUDIO_WRITE_PROTECTION,     /* 19 */
    ANT_AUDIO_PARAM_ERROR,          /* 20 */
    ANT_AUDIO_UNSUPPORTED_CHANNEL,  /* 21 */
    ANT_AUDIO_UNSUPPORTED_FREQ,     /* 22 */
    ANT_AUDIO_UNSUPPORTED_TYPE,     /* 23 */
    ANT_AUDIO_UNSUPPORTED_OPERATION,/* 24 */
    ANT_AUDIO_PARSER_ERROR          /* 25 */
} ant_audio_play_result_e;

typedef void (* ant_audio_play_finish_callback_t)(ant_u16 play_id);

void ant_audio_init(void);

/**
 * 注册播放完成回调函数
 *
 * @param cb 播放完成时回调cb，入参是ant_audio_play_file或ant_audio_play_buffer传入的play_id
 * @return
 */
ant_s32 ant_register_play_callback(ant_audio_play_finish_callback_t cb);

/**
 * 播放音频文件：MP3, WAV格式
 *
 * @param filename 音频文件路径
 * @param play_id  音频播放请求id
 *
 * @return 0 播放成功
 *         非0值： 播放失败，取值参考ant_audio_play_result_e
 */
ant_s32 ant_audio_play_file(char *filename, ant_u16 play_id);

/**
 * 播放缓存数据：MP3, WAV格式
 *
 * @param data 音频数据缓存
 * @param len 音频缓存数据长度
 * @param format 音频格式，取值参见ant_audio_format_e
 * @param play_id 音频播放请求id
 * @return 0 播放成功
 *         非0值： 播放失败，取值参考ant_audio_play_result_e
 */
ant_s32 ant_audio_play_buffer(ant_u8 *data, ant_u32 len, int format, ant_u16 play_id);

/**
 * 设置播放音量
 *
 * @param volume 音量大小 0~7
 *
 * @return 0 播放成功
 *         非0值： 播放失败，取值参考ant_audio_play_result_e
 */
ant_s32 ant_audio_set_volume(ant_u8 volume);

/**
 * 获取播放音量
 *
 * @return 音量大小，最小值ANT_AUDIO_MIN_VOLUME，最大值ANT_AUDIO_MAX_VOLUME
 */
ant_s32 ant_audio_get_volume(void);

/**
 * 设置静音
 *
 * @return   0 成功
 *          -1 失败
 */
ant_s32 ant_audio_mute(void);

/**
 * 取消静音
 *
 * @return   0 成功
 *          -1 失败
 */
ant_s32 ant_audio_unmute(void);

/**
 * 获取文件系统中的音频文件格式和数据
 *
 * @param filename 音频文件路径
 * @param aud_len 返回音频数据长度
 * @param format 返回音频数据格式，参见ant_audio_format_e
 * @return 音频数据flash缓存地址
 */
ant_u8 *ant_audio_get_data(const char *filename, ant_u32 *aud_len, ant_s32 *format);


#ifdef __cplusplus
}
#endif
#endif
