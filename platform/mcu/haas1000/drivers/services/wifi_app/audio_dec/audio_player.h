/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        AUDIO_PLAYER_FORMAT_INVALID = -1,
        AUDIO_PLAYER_FORMAT_MP3 = 0,
        AUDIO_PLAYER_FORMAT_AAC_ADTS,
        AUDIO_PLAYER_FORMAT_AAC_M4A,
        AUDIO_PLAYER_FORMAT_OPUS,
        AUDIO_PLAYER_FORMAT_PCM,
        AUDIO_PLAYER_FORMAT_WAV,
    } audio_player_format_t;

    int audio_player_init(void);
    typedef void (*audio_player_playback_cb)(void);

    /* if player pcm audio, should config pcm arg. */
    int audio_player_fomat_pcm_config(uint32_t sample_rate, uint8_t channels_num);

    /* audio_player play audio from file, auto start player. */
    int audio_player_play_file(const char *path_name, audio_player_playback_cb cb);

    /* audio_player play audio from memory data, audo start player. */
    int audio_player_play_mem_init(audio_player_format_t format, uint32_t total_size, audio_player_playback_cb cb);
    int audio_player_play_mem(const uint8_t *data, uint32_t data_len);
    int audio_player_play_mem_denint(void);

    /* audio_player control function */
    int audio_player_stop(void);
#if 0 
    /* Not Support */
    int audio_player_pause(void);
    int audio_player_resume(void);
#endif

    /* for filesystem */
    typedef int (*audio_player_file_open)(const char *pathname, int flags);
    typedef int (*audio_player_file_close)(int fd);
    typedef int (*audio_player_file_read)(int fd, void *buf, uint32_t count);

    typedef struct {
        audio_player_file_open open;
        audio_player_file_close close;
        audio_player_file_read read;
    } audio_player_file_opt_t;

    int audio_player_file_ops_register(audio_player_file_opt_t * file_opt);
    int audio_fs_open(const char *pathname, int flags);
    int audio_fs_close(int fd);
    uint32_t audio_fs_read(int fd, void *buf, uint32_t count);

    /* some test case */
    int audio_player_test_from_sd_card_file(const char *path);
    int audio_player_test_from_sd_card_file_and_stop(const char *path, uint32_t play_time_ms);
    int audio_player_test_from_mem(audio_player_format_t foramt, uint8_t *raw_data, uint32_t raw_data_size);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_PLAYER_H__ */
