/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"

static os_task_t play_task;

static uvoice_player_t *uvocplayer;

static char text_source[256];

static int source_sample_rate;
static int source_channels;

static int get_format_from_name(char *name, media_format_t *format)
{
    if (!name || !format) {
        M_LOGE("arg null !\n");
        return -1;
    }

    if (strstr(name, ".mp3") || strstr(name, ".MP3"))
        *format = MEDIA_FMT_MP3;
    else if (strstr(name, ".wav") || strstr(name, ".WAV"))
        *format = MEDIA_FMT_WAV;
    else if (strstr(name, ".aac") || strstr(name, ".AAC"))
        *format = MEDIA_FMT_AAC;
    else if (strstr(name, ".m4a") || strstr(name, ".M4A"))
        *format = MEDIA_FMT_M4A;
    else if (strstr(name, ".pcm") || strstr(name, ".PCM"))
        *format = MEDIA_FMT_PCM;
    else if (strstr(name, ".spx") || strstr(name, ".SPX"))
        *format = MEDIA_FMT_SPX;
    else if (strstr(name, ".ogg") || strstr(name, ".OGG"))
        *format = MEDIA_FMT_OGG;
    else if (strstr(name, ".amrwb") || strstr(name, ".AMRWB"))
        *format = MEDIA_FMT_AMRWB;
    else if (strstr(name, ".amr") || strstr(name, ".AMR"))
        *format = MEDIA_FMT_AMR;
    else if (strstr(name, ".opus") || strstr(name, ".OPUS"))
        *format = MEDIA_FMT_OPS;
    else if (strstr(name, ".flac") || strstr(name, ".FLAC"))
        *format = MEDIA_FMT_FLAC;

    return 0;
}

static void uvoice_st_event_cb(uvoice_event_t *event, void *data)
{
    if (event->type != UVOICE_EV_ST)
        return;

    if (event->code == UVOICE_CODE_VAD_START)
        printf("vad ---->begin\n");
    else if (event->code == UVOICE_CODE_VAD_END)
        printf("vad ---->end\n");
}

/* usage: play sine 16000 16 1 1000 */
static int play_sine_wave(int rate, int bits, int channels, int freq_hz)
{
#define PI                    3.14159265
#define CYCLES_PER_PERIOD    8
    if (freq_hz == 0) {
        return -1;
    }
    unsigned int samples_per_cycle = rate / freq_hz;
    unsigned int samples_per_period = samples_per_cycle * CYCLES_PER_PERIOD;
    unsigned int sample_size = (bits >> 3) * channels;

    if (bits != 8 && bits != 16 && bits != 24) {
        M_LOGE("bits %d invalid !\n", bits);
        return -1;
    }

    if (channels != 1 && channels != 2) {
        M_LOGE("channels %d invalid !\n", channels);
        return -1;
    }

    char *samples_data = snd_zalloc(sample_size * samples_per_cycle, AFM_MAIN);
    if (!samples_data) {
        M_LOGE("alloc samples_data failed !\n");
        return -1;
    }

    char *period_buffer = snd_zalloc(sample_size * samples_per_period, AFM_MAIN);
    if (!period_buffer) {
        M_LOGE("alloc period_buffer failed !\n");
        snd_free(samples_data);
        return -1;
    }

    unsigned int msec_per_period = (samples_per_period * 1000) / rate;
    unsigned int play_msec = 0;
    unsigned int sample_val;
    unsigned int sample_idx;
    unsigned int cycle_idx;

    double sin_float;
    double triangle_float;
    double triangle_step = (double)pow(2, bits) / samples_per_cycle;

    M_LOGI("sample_size %d samper_per_cycly %d msec_per_period %d\n",
        sample_size, samples_per_cycle, msec_per_period);

    triangle_float = -(pow(2, bits) / 2 - 1);

    for (sample_idx = 0; sample_idx < samples_per_cycle; sample_idx++) {
        sin_float = sin(sample_idx * PI / 180.0);
        if (sin_float >= 0)
            triangle_float += triangle_step;
        else
            triangle_float -= triangle_step;

        sin_float *= (pow(2, bits) / 2 - 1);

        if (bits == 16) {
            sample_val = 0;
            sample_val += (short)triangle_float;
            sample_val = sample_val << 16;
            sample_val += (short)sin_float;
            samples_data[sample_idx] = sample_val;
        } else if (bits == 24) {
            samples_data[sample_idx * 2] = ((int)triangle_float) << 8;
            samples_data[sample_idx * 2 + 1] = ((int)sin_float) << 8;
        } else {
            samples_data[sample_idx * 2] = (int)triangle_float;
            samples_data[sample_idx * 2 + 1] = (int)sin_float;
        }
    }

    for (cycle_idx = 0; cycle_idx < CYCLES_PER_PERIOD; cycle_idx++) {
        memcpy(period_buffer + cycle_idx * (sample_size * samples_per_cycle),
            samples_data,
            samples_per_cycle * sample_size);
    }

    M_LOGI("start\n");

    player_state_t player_state = -1;
    bool player_paused = false;
    uvocplayer->get_state(&player_state);
    if (player_state == PLAYER_STAT_RUNNING ||
        player_state == PLAYER_STAT_COMPLETE) {
        uvocplayer->set_fade(40, 60);
        uvocplayer->pause();
        player_paused = true;
    }

    uvocplayer->set_stream(MEDIA_FMT_PCM, 0, 0);
    uvocplayer->set_pcminfo(rate, channels, bits, samples_per_period);

    while (play_msec <= 5000) {
        uvocplayer->put_stream(period_buffer, sample_size * samples_per_period);
        play_msec += msec_per_period;
    }

    uvocplayer->clr_stream(1);

    if (player_paused) {
        uvocplayer->set_fade(40, 60);
        uvocplayer->resume();
        player_paused = false;
    }

    snd_free(period_buffer);
    snd_free(samples_data);

    M_LOGI("exit\n");
    return 0;
}

static void play_music(void *arg)
{
    media_format_t format = MEDIA_FMT_UNKNOWN;
    get_format_from_name(text_source, &format);

    if (uvocplayer->set_source(text_source)) {
        M_LOGE("set source failed !\n");
        os_task_exit(NULL);
        return;
    }

    if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX)
        uvocplayer->set_pcminfo(source_sample_rate, source_channels, 16, 1280);

    if (uvocplayer->start()) {
        M_LOGE("start failed !\n");
        uvocplayer->clr_source();
    }

    os_task_exit(NULL);
}

static void play_loopback(void *arg)
{
#ifdef UVOICE_RECORDER_ENABLE
    #define LOOPBACK_TEST_DURATION_SEC    20
    unsigned char *read_buffer;
    int read_samples = 640;
    int channels = 2;
    int rate = 32000;
    int bits = 16;
    int buffer_size;
    int i = 0;

    int count = (LOOPBACK_TEST_DURATION_SEC * 1000) / (read_samples / (rate / 1000));

    buffer_size = read_samples * channels * (bits >> 3);
    read_buffer = malloc(buffer_size);
    if (!read_buffer) {
        M_LOGE("alloc read buffer failed !\n");
        os_task_exit(NULL);
        return;
    }

    uvoice_recorder_t *mrecorder = uvoice_recorder_create();
    if (!mrecorder) {
        M_LOGE("create uvoice recorder failed !\n");
        free(read_buffer);
        os_task_exit(NULL);
        return;
    }

    if (mrecorder->set_sink(MEDIA_FMT_PCM, rate, channels, bits, read_samples, 0, NULL)) {
        M_LOGE("set record sink failed !\n");
        uvoice_recorder_release(mrecorder);
        free(read_buffer);
        os_task_exit(NULL);
        return;
    }

    uvocplayer->set_stream(MEDIA_FMT_PCM, 0, 0);
    uvocplayer->set_pcminfo(rate, channels, bits, read_samples);
    while (++i < count) {
        mrecorder->get_stream(read_buffer, buffer_size);
        uvocplayer->put_stream(read_buffer, buffer_size);
    }
    uvocplayer->clr_stream(0);

    mrecorder->clr_sink();
    uvoice_recorder_release(mrecorder);
    free(read_buffer);
#endif
    os_task_exit(NULL);
}

static void play_list_scan(void *arg)
{
#if (UVOICE_MLIST_ENABLE == 1)
    mlist_source_scan();
#endif
    os_task_exit(NULL);
}

static void play_list_del(void *arg)
{
#if (UVOICE_MLIST_ENABLE == 1)
    mlist_source_del((int)arg);
#endif
    os_task_exit(NULL);
}

static void play_list_set(void *arg)
{
#if (UVOICE_MLIST_ENABLE == 1)
    mlist_index_set((int)arg);
#endif
    os_task_exit(NULL);
}

static void play_equalizer_config(char *fc, char *width, char *gain)
{
#ifdef UVOICE_EQ_ENABLE
extern char equalizer_opt_fc[16];
extern char equalizer_opt_width[16];
extern char equalizer_opt_gain[16];
    snprintf(equalizer_opt_fc, 16, "%s", fc);
    snprintf(equalizer_opt_width, 16, "%s", width);
    snprintf(equalizer_opt_gain, 16, "%s", gain);
#endif
}

static void play_number(char *num)
{
#ifdef UVOICE_COMB_ENABLE
    comb_init();
    comb_alipay_number(atof(num));
#endif
}

static void play_music_stream(void *arg)
{
    media_format_t format = MEDIA_FMT_UNKNOWN;
    FILE *fp;
    int buffer_size = 2048;
    char *buffer;
    int ret;

    get_format_from_name(text_source, &format);

    fp = fopen(text_source, "rb");
    if (!fp) {
        M_LOGE("open %s failed !\n", text_source);
        os_task_exit(NULL);
        return;
    }

    buffer = malloc(buffer_size);
    if (!buffer) {
        M_LOGE("alloc buffer failed !\n");
        fclose(fp);
        os_task_exit(NULL);
        return;
    }

    ret = fread(buffer, 1, buffer_size, fp);
    if (ret <= 0) {
        M_LOGE("read failed %d!\n", ret);
        fclose(fp);
        free(buffer);
        os_task_exit(NULL);
        return;
    } else if (ret != buffer_size) {
        M_LOGI("read %d ret %d\n", buffer_size, ret);
    }

    if (uvocplayer->set_stream(format, 0, 8192)) {
        fclose(fp);
        free(buffer);
        os_task_exit(NULL);
        return;
    }

    if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX)
        uvocplayer->set_pcminfo(source_sample_rate, source_channels, 16, 320);

    while (ret > 0) {
        if (uvocplayer->put_stream(buffer, ret))
            break;
        ret = fread(buffer, 1, buffer_size, fp);
        if (ret < 0) {
            M_LOGE("read failed %d!\n", ret);
            uvocplayer->clr_stream(1);
            fclose(fp);
            free(buffer);
            os_task_exit(NULL);
            return;
        } else if (ret == 0) {
            M_LOGI("read end\n");
            break;
        } else if (ret != buffer_size) {
            M_LOGI("read %d ret %d\n", buffer_size, ret);
        }
    }

    uvocplayer->clr_stream(0);

    fclose(fp);
    free(buffer);
    os_task_exit(NULL);
}

void uvoice_play_test(int argc, char **argv)
{
    if (!uvocplayer) {
        uvocplayer = uvoice_player_create();
        if (!uvocplayer) {
            M_LOGE("create media player failed !\n");
            return;
        }
        uvocplayer->eq_enable(0);

        uvoice_event_register(UVOICE_EV_ST, uvoice_st_event_cb, NULL);
    }

    if (argc < 2 || strcmp(argv[0], "play"))
        return;

    if (!strcmp(argv[1], "stop")) {
        uvocplayer->set_fade(40, 40);
        uvocplayer->stop_async();
    } else if (!strcmp(argv[1], "stopsync")) {
        uvocplayer->set_fade(40, 40);
        uvocplayer->stop();
    } else if (!strcmp(argv[1], "source")) {
        if (argc < 3)
            return;
        uvocplayer->set_source(argv[2]);
    } else if (!strcmp(argv[1], "start")) {
        uvocplayer->start();
    } else if (!strcmp(argv[1], "clear")) {
        uvocplayer->clr_source();
    } else if (!strcmp(argv[1], "pause")) {
        uvocplayer->set_fade(40, 40);
        uvocplayer->pause_async();
    } else if (!strcmp(argv[1], "pausesync")) {
        uvocplayer->set_fade(40, 40);
        uvocplayer->pause();
    } else if (!strcmp(argv[1], "resume")) {
        uvocplayer->set_fade(40, 40);
        uvocplayer->resume();
    } else if (!strcmp(argv[1], "complete")) {
        uvocplayer->complete();
    } else if (!strcmp(argv[1], "playback")) {
        if (argc < 3)
            return;
        uvocplayer->playback(argv[2]);
    } else if (!strcmp(argv[1], "dump")) {
        uvocplayer->state_dump();
    } else if (!strcmp(argv[1], "num")) {
        if (argc < 3)
            return;
        play_number(argv[2]);
    } else if (!strcmp(argv[1], "pcmdump")) {
        uvocplayer->pcmdump_enable(atoi(argv[2]));
    } else if (!strcmp(argv[1], "fade")) {
        if (argc < 4)
            return;
        uvocplayer->set_fade(atoi(argv[2]), atoi(argv[3]));
    } else if (!strcmp(argv[1], "sine")) {
        if (argc < 6)
            return;
        play_sine_wave(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    } else if (!strcmp(argv[1], "progress")) {
        int position = 0;
        int duration = 0;
        uvocplayer->get_position(&position);
        uvocplayer->get_duration(&duration);
        printf("progress: %d/%d\n", position, duration);
    } else if (!strcmp(argv[1], "seek")) {
        if (argc < 3)
            return;
        uvocplayer->seek(atoi(argv[2]));
    } else if (!strcmp(argv[1], "format")) {
        if (argc < 3)
            return;
        uvocplayer->set_format(atoi(argv[2]));
    } else if (!strcmp(argv[1], "standby")) {
        if (argc < 3)
            return;
        uvocplayer->set_standby(atoi(argv[2]));
    } else if (!strcmp(argv[1], "state")) {
        player_state_t state = -1;
        uvocplayer->get_state(&state);
        printf("state %d\n", state);
    } else if (!strcmp(argv[1], "loopback")) {
        os_task_create(&play_task,
            "loopback task", play_loopback,
            NULL, 8192, 0);
    } else if (!strcmp(argv[1], "eq")) {
        if (argc >= 5) {
            #ifdef UVOICE_EQ_ENABLE
            play_equalizer_config(argv[2], argv[3], argv[4]);
            #endif
            return;
        }
        uvocplayer->eq_enable(atoi(argv[2]));
    } else if (!strcmp(argv[1], "list")) {
        #if (UVOICE_MLIST_ENABLE == 1)
        if (argc == 2) {
            mlist_source_show();
            return;
        }

        if (argc == 4 && !strcmp(argv[2], "index") && !strcmp(argv[3], "get")) {
            int index = 0;
            if (mlist_index_get(&index)) {
                M_LOGE("get index failed !\n");
                return;
            }
            M_LOGI("current source index %d\n", index);
            return;
        }

        if (argc == 5 && !strcmp(argv[2], "index") && !strcmp(argv[3], "set")) {
            os_task_create(&play_task,
                "list set task", play_list_set,
                atoi(argv[4]), 4096, 0);
            return;
        }

        if (argc == 3 && !strcmp(argv[2], "scan")) {
            os_task_create(&play_task,
                "player scan task", play_list_scan,
                NULL, 4096, 0);
            return;
        }

        if (argc == 3) {
            int index = atoi(argv[2]);
            char file_path[128];
            memset(file_path, 0, sizeof(file_path));
            if (mlist_source_get(index, file_path, sizeof(file_path))) {
                M_LOGE("search list failed !\n");
                return;
            }
            uvocplayer->set_source(file_path);
            uvocplayer->start();
            return;
        }

        if (argc == 4 && !strcmp(argv[2], "del")) {
            os_task_create(&play_task,
                "player del task", play_list_del,
                atoi(argv[3]), 4096, 0);
            return;
        }
        #endif
    } else if (!strcmp(argv[1], "cacheinfo")) {
        int cache_len = 0;
        uvocplayer->get_cacheinfo(&cache_len);
        printf("avail cache len %d\n", cache_len);
    } else if (!strcmp(argv[1], "download")) {
        if (argc >= 3) {
            if (!strcmp(argv[2], "abort"))
                uvocplayer->download_abort();
            else
                uvocplayer->download(argv[2]);
        } else {
            uvocplayer->download(NULL);
        }
    } else if (!strcmp(argv[1], "stream")) {
        if (argc < 3)
            return;
        if (!strcmp(argv[2], "stop")) {
            uvocplayer->clr_stream(1);
            return;
        }
        memset(text_source, 0, sizeof(text_source));
        snprintf(text_source, sizeof(text_source), "%s", argv[2]);
        media_format_t format = MEDIA_FMT_UNKNOWN;
        get_format_from_name(text_source, &format);
        if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX) {
            if (argc < 5) {
                M_LOGE("args too less for this format\n");
                return;
            }
            source_sample_rate = atoi(argv[3]);
            source_channels = atoi(argv[4]);
        }
        os_task_create(&play_task,
            "stream play task", play_music_stream,
            NULL, 4096, 0);
    } else if (!strcmp(argv[1], "volume")) {
        if (argc >= 3) {
            uvocplayer->set_volume(atoi(argv[2]));
        } else {
            int volume = 0;
            uvocplayer->get_volume(&volume);
            printf("volume %d\n", volume);
        }
    } else {
        memset(text_source, 0, sizeof(text_source));
        snprintf(text_source, sizeof(text_source), "%s", argv[1]);
        media_format_t format = MEDIA_FMT_UNKNOWN;
        get_format_from_name(text_source, &format);
        if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX) {
            if (argc < 4) {
                M_LOGE("args too less for this format\n");
                return;
            }
            source_sample_rate = atoi(argv[2]);
            source_channels = atoi(argv[3]);
        }
        os_task_create(&play_task,
            "play music task", play_music,
            NULL, 8192, 0);
    }
}

