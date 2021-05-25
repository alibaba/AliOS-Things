/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_STREAM_H__
#define __AUDIO_STREAM_H__


enum stream_state {
    STREAM_STOP = 0,
    STREAM_SETUP,
    STREAM_START,
    STREAM_RUNNING,
};

struct noise_suppression {
    void *handler;
};

struct auto_gain_control {
    void *handler;
};

struct out_stream {
    #ifdef AUDIO_CACHE_ENABLE
    uint8_t *buffer;
    int buffer_size;
    int buffer_dirty_size;
    #endif
    uint8_t silent:1;
    enum stream_state state;
    void *eq;
    FILE *dump;
    os_mutex_t lock;
    struct pcm_device pcm;
};

struct in_stream {
    enum stream_state state;
    os_mutex_t lock;
    struct noise_suppression *ns;
    struct auto_gain_control *agc;
    void *aec;
    void *vad;
    struct pcm_device pcm;
};

struct audio_device {
    struct out_stream *out;
    struct in_stream *in;
    uint8_t noise_suppression:1;
    uint8_t echo_cancellation:1;
    uint8_t auto_gain_control:1;
    uint8_t vad_enable:1;
    uint8_t equalizer_enable:1;
    uint8_t out_pcm_dump:1;
    uint8_t vad_start:1;
    uint8_t in_device_fix:1;
    int out_volume;
    snd_device_t out_device;
    snd_device_t in_device;
    snd_device_t fixed_out_device;
};

int out_volume_set(int volume);
int out_volume_get(void);
int out_volume_range(int *max, int *min);
int out_device_set(snd_device_t device);
int out_extpa_set(audio_extpa_info_t *info);
int out_stream_reset(struct out_stream *out);
int out_stream_mute(struct out_stream *out, int mute);
bool out_stream_configured(struct out_stream *out);
int out_stream_configure(struct out_stream *out, media_pcminfo_t *pcminfo);
int out_stream_period_duration(struct out_stream *out);
int out_stream_write(struct out_stream *out, uint8_t *buffer, int nbytes);
int out_stream_silent(struct out_stream *out);
int out_stream_stop(struct out_stream *out);
struct out_stream *out_stream_create(void);
int out_stream_release(struct out_stream *out);
bool in_stream_configured(struct in_stream *in);
int in_stream_configure(struct in_stream *in, media_pcminfo_t *pcminfo);
int in_stream_read(struct in_stream *in, uint8_t *buffer, int nbytes);
int in_stream_stop(struct in_stream *in);
struct in_stream *in_stream_create(void);
int in_stream_release(struct in_stream *in);
int audio_param_set(int key, int value);
int audio_device_init(void);
int audio_device_deinit(void);


#endif /* __AUDIO_STREAM_H__ */
