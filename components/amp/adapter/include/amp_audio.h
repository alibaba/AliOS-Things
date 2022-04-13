/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* JSEngine OS Abstract Layer */
#ifndef AMP_AUDIO_H
#define AMP_AUDIO_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "stdarg.h"
#include "infra_types.h"
#include "wrappers_defs.h"

typedef enum {
    AUDIO_FORMAT_UNKNOWN = 0,
    AUDIO_FORMAT_PCM,
    AUDIO_FORMAT_WAV,
    AUDIO_FORMAT_MP3,
    AUDIO_FORMAT_AAC,
    AUDIO_FORMAT_M4A,
    AUDIO_FORMAT_OGG,
} amp_audio_format_t;

typedef enum {
	TEXT_ENCODE_TYPE_UTF8 = 0,
	TEXT_ENCODE_TYPE_GB2312,
} amp_text_encode_type_t;

/**
 * Audio playback start interface
 *
 * @param[in]  source  audio source path or url
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_play(const char *source);

/**
 * Audio playback stop interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_stop(void);

/**
 * Audio playback pause interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_pause(void);

/**
 * Audio playback resume interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_resume(void);

/**
 * Audio mic gain set interface
 *
 * @param[in]  gain  mic gain value
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_mic_gain_set(int gain);

/**
 * Audio mic gain set interface
 *
 * @param[in]  gain  pointer to the gain data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_mic_gain_get(int *gain);

/**
 * Audio playback volume set interface
 *
 * @param[in]  device  audio device
 * @param[in]  volume  volume level
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_volume_set(int device, int volume);

/**
 * Audio playback mute control interface
 *
 * @param[in]  state  mute on/off
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_audio_mute(int state);

/**
 * TTS playback interface
 *
 * @param[in]  text  text to play
 * @param[in]  type  text encode format
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_tts_play(const char *text, amp_text_encode_type_t type);

/**
 * TTS playback stop interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_tts_stop(void);

/**
 * TTS playback volume set interface
 *
 * @param[in]  volume  volume level
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_tts_volume_set(int volume);

/**
 * TTS playback pitch set interface
 *
 * @param[in]  pitch  pitch setting
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_tts_pitch_set(int pitch);

/**
 * TTS playback speed set interface
 *
 * @param[in]  speed  speed level
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t amp_hal_tts_speed_set(int speed);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t amp_hal_tts_is_playing(void);


#if defined(__cplusplus)
}
#endif

#endif /* AMP_AUDIO_H */
