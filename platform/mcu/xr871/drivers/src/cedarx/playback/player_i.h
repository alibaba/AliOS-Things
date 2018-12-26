/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : player_i.h
 * Description : player_i
 * History :
 *
 */

#ifndef PLAYER_I_H
#define PLAYER_I_H

#include "player.h"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

enum EPLAYERINTERNALNOTIFY  //* player internal notify.
{
    PLAYER_VIDEO_DECODER_NOTIFY_STREAM_UNDERFLOW        = 32,
    PLAYER_VIDEO_DECODER_NOTIFY_CRASH,
    PLAYER_VIDEO_DECODER_NOTIFY_EOS,
    PLAYER_VIDEO_DECODER_NOTIFY_SET_SECURE_BUFFER_COUNT,
    PLAYER_VIDEO_DECODER_NOTIFY_SET_SECURE_BUFFERS,

    PLAYER_VIDEO_RENDER_NOTIFY_FIRST_PICTURE            = 64,
    PLAYER_VIDEO_RENDER_NOTIFY_PICTURE_PTS,
    PLAYER_VIDEO_RENDER_NOTIFY_EOS,
    PLAYER_VIDEO_RENDER_NOTIFY_VIDEO_SIZE,
    PLAYER_VIDEO_RENDER_NOTIFY_VIDEO_CROP,
    PLAYER_VIDEO_RENDER_NOTIFY_VIDEO_FRAME,
    PLAYER_VIDEO_RENDER_NOTIFY_CRASH,

    PLAYER_AUDIO_DECODER_NOTIFY_STREAM_UNDERFLOW        = 96,
    PLAYER_AUDIO_DECODER_NOTIFY_CRASH,
    PLAYER_AUDIO_DECODER_NOTIFY_EOS,
    PLAYER_AUDIO_DECODER_NOTIFY_AUDIORAWPLAY,

    PLAYER_AUDIO_RENDER_NOTIFY_FIRST_FRAME              = 128,
    PLAYER_AUDIO_RENDER_NOTIFY_EOS,
    PLAYER_AUDIO_RENDER_NOTIFY_PTS_AND_CACHETIME,
    PLAYER_AUDIO_RENDER_NOTIFY_AUDIO_INFO,

    PLAYER_SUBTITLE_DECODER_NOTIFY_STREAM_UNDERFLOW     = 160,
    PLAYER_SUBTITLE_DECODER_NOTIFY_CRASH,
    PLAYER_SUBTITLE_DECODER_NOTIFY_EOS,

    PLAYER_SUBTITLE_RENDER_NOTIFY_ITEM_PTS_AND_DURATION = 192,
    PLAYER_SUBTITLE_RENDER_NOTIFY_ITEM_AVAILABLE,
    PLAYER_SUBTITLE_RENDER_NOTIFY_ITEM_EXPIRED,
    PLAYER_SUBTITLE_RENDER_NOTIFY_EOS,
};

// report every 5 seconds.
#define CONFIG_AUDIO_RENDER_TIME_DIFFERENCE_REPORT_PERIOD       (5000000)

// choose the larger pts of nFirstVideoPts and nFirstAudioPts as the player start time.
#define CONFIG_USE_LARGER_PTS_AS_START_TIME                     (1)

// if pts difference is more than 4 seconds, we judge there is a pts jump(loop back) event.
#define CONFIG_THREASHOLD_OF_PTS_DIFFERENCE_TO_JUDGE_PTS_jUMP   (4000000)

// if pts difference is more than 1.5 seconds, we judge there is a pts jump(loop back) event.
#define CONFIG_THREASHOLD_OF_PTS_DIFFERENCE_TO_JUDGE_PTS_jUMP_DTMB   (1500000)

/* don't wait for synchronization for too long, should not smaller than
 * CONFIG_THREASHOLD_OF_PTS_DIFFERENCE_TO_JUDGE_PTS_jUMP.
 */
#define CONFIG_MAX_WAIT_TIME_FOR_SYNC                           (4000000)

// 20 seconds.
#define CONFIG_THREASHOLD_OF_PTS_DIFFERENCE_TO_JUDGE_PTS_jUMP_FOR_SUBTITLE (20000000)

#define TIMER_DROP_AUDIO_DATA       0x04
#define TIMER_DROP_VIDEO_DATA       0x05
#define TIMER_NEED_NOTIFY_AGAIN     0x06

/*static int64_t GetSysTime(void)
{
    int64_t time;
    struct timeval t;
    gettimeofday(&t, NULL);
    time = (int64_t)t.tv_sec * 1000000;
    time += t.tv_usec;
    return time;
}*/

#define CDX_PLAYER_UNUSE(param) (void)param

#endif
