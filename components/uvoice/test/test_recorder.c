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
#include "uvoice_recorder.h"

#include "uvoice_os.h"


#define APP_LOGI(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)


static uvoice_recorder_t *uvocrecorder;
static os_task_t record_task;

static int rec_rate;
static int rec_channels;
static int rec_bits;
static int rec_frames;
static int rec_bitrate;
static char rec_sink[64];

static int get_format_from_name(char *name, media_format_t *format)
{
    if (!name || !format) {
        APP_LOGE("arg null !\n");
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

static void record_to_file(void)
{
    uvocrecorder->set_sink(MEDIA_FMT_UNKNOWN, rec_rate, rec_channels,
        rec_bits, rec_frames, rec_bitrate, rec_sink);
    uvocrecorder->start();
    os_task_exit(NULL);
}

void uvoice_record_test(int argc, char **argv)
{
    if (!uvocrecorder) {
        uvocrecorder = uvoice_recorder_create();
        if (!uvocrecorder) {
            APP_LOGE("create media recorder failed !\n");
            return;
        }
    }

    if (argc < 2 || strcmp(argv[0], "record"))
        return;

    if (!strcmp(argv[1], "stop")) {
        uvocrecorder->stop();
    } else if (!strcmp(argv[1], "start")) {
        uvocrecorder->start();
    } else if (!strcmp(argv[1], "clear")) {
        uvocrecorder->clr_sink();
    } else if (!strcmp(argv[1], "progress")) {
        int position = 0;
        uvocrecorder->get_position(&position);
        printf("progress: %d\n", position);
    } else if (!strcmp(argv[1], "state")) {
        recorder_state_t state = -1;
        uvocrecorder->get_state(&state);
        printf("state %d\n", state);
    } else if (!strcmp(argv[1], "ns")) {
        if (argc < 3)
            return;
        if (!strcmp(argv[2], "enable"))
            uvocrecorder->ns_enable(1);
        else if (!strcmp(argv[2], "disable"))
            uvocrecorder->ns_enable(0);
    } else if (!strcmp(argv[1], "ec")) {
        if (argc < 3)
            return;
        if (!strcmp(argv[2], "enable"))
            uvocrecorder->ec_enable(1);
        else if (!strcmp(argv[2], "disable"))
            uvocrecorder->ec_enable(0);
    } else if (!strcmp(argv[1], "agc")) {
        if (argc < 3)
            return;
        if (!strcmp(argv[2], "enable"))
            uvocrecorder->agc_enable(1);
        else if (!strcmp(argv[2], "disable"))
            uvocrecorder->agc_enable(0);
    } else if (!strcmp(argv[1], "vad")) {
        if (argc < 3)
            return;
        if (!strcmp(argv[2], "enable"))
            uvocrecorder->vad_enable(1);
        else if (!strcmp(argv[2], "disable"))
            uvocrecorder->vad_enable(0);
    } else {
        if (argc < 7)
            return;
        rec_rate = atoi(argv[1]);
        rec_channels = atoi(argv[2]);
        rec_bits = atoi(argv[3]);
        rec_frames = atoi(argv[4]);
        rec_bitrate = atoi(argv[5]);
        memset(rec_sink, 0, sizeof(rec_sink));
        snprintf(rec_sink, sizeof(rec_sink), "%s", argv[6]);

        os_task_create(&record_task,
            "reord test task",
            record_to_file, NULL, 4096, 0);
    }
}

