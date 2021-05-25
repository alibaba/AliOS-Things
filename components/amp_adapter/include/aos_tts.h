/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __AOS_TTS_H__
#define __AOS_TTS_H__

typedef enum {
    TEXT_ENCODE_TYPE_UTF8 = 1,
    TEXT_ENCODE_TYPE_GBK,
} aos_text_encode_type_t;

typedef enum {
    AOS_TTS_EVENT_INIT = 0,
    AOS_TTS_EVENT_DEINIT,
    AOS_TTS_EVENT_PLAY_START,
    AOS_TTS_EVENT_PLAY_STOP,
    AOS_TTS_EVENT_PLAY_COMPLETE,
    AOS_TTS_EVENT_PLAY_FAIL,
    AOS_TTS_EVENT_PLAY_INTERRUPT,
} aos_tts_event_t;


/**
 * TTS playback interface
 *
 * @param[in]  text  text to play
 * @param[in]  type  text encode format
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_play(const char *text, aos_text_encode_type_t type);

/**
 * TTS playback stop interface
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_stop(void);

/**
 * TTS state get interface
 *
 * @param[in]  volume  memory address to store state value
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_state_get(int *state);

/**
 * TTS playback volume set interface
 *
 * @param[in]  volume  volume level
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_volume_set(int volume);

/**
 * TTS playback volume get interface
 *
 * @param[in]  volume  memory address to store volume value
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_volume_get(int *volume);

/**
 * TTS playback pitch set interface
 *
 * @param[in]  pitch  pitch setting
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_pitch_set(int pitch);

/**
 * TTS playback speed set interface
 *
 * @param[in]  speed  speed level
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_speed_set(int speed);

/**
 * TTS playback speed get interface
 *
 * @param[in]  speed  memory address to store speed value
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_speed_get(int *speed);

/**
 * TTS playback out device set
 *
 * @param[in]  device  audio out device selected by user
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_set_out_device(aos_snd_device_t device);

/**
 * TTS playback external pa control config
 *
 * @param[in]  pin  gpio pin number used to turn on/off external pa
 * @param[in]  active_high  pin level when external pa is on
 * @param[in]  delay_ms  delay in ms after pa is on
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_set_external_pa(int pin, int active_high, int delay_ms);

/**
 * Get tts playing state
 *
 * @return  0 : no tts playing, 1 : tts playing
 */
int aos_tts_is_playing(void);

/**
 * TTS system init
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_init(void (*cb)(aos_tts_event_t event, char *content));

/**
 * TTS system deinit
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_tts_deinit(void);


#endif /* __AOS_TTS_H__ */
