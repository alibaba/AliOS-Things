/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __PRJ_CONFIG_XPLAYER

#include "cmd_util.h"

//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pthread.h"
//#include <ctype.h>
//#include "errno.h"
//#include <sys/select.h>

#include "iniparserapi.h"

//#include "cdx_config.h"
#include <cdx_log.h>
#include "xplayer.h"
#include "CdxTypes.h"
#include "fs/fatfs/ff.h"
//#include "memoryAdapter.h"
//#include "deinterlace.h"
//typedef unsigned long uintptr_t ;
extern SoundCtrl* SoundDeviceCreate();

static const int STATUS_STOPPED   = 0;
static const int STATUS_PREPARING = 1;
static const int STATUS_PREPARED  = 2;
static const int STATUS_PLAYING   = 3;
static const int STATUS_PAUSED    = 4;
static const int STATUS_SEEKING   = 5;

typedef struct DemoPlayerContext
{
    XPlayer*       mAwPlayer;
    int             mPreStatus;
    int             mStatus;
    int             mSeekable;
    int             mError;
    pthread_mutex_t mMutex;
    sem_t           mStoped;
    sem_t           mPrepared;
//    int             mVideoFrameNum;
}DemoPlayerContext;


//* define commands for user control.
typedef struct Command
{
    const char* strCommand;
    int         nCommandId;
    const char* strHelpMsg;
}Command;

#define COMMAND_HELP            0x1     //* show help message.
#define COMMAND_QUIT            0x2     //* quit this program.

#define COMMAND_SET_SOURCE      0x101   //* set url of media file.
#define COMMAND_PLAY            0x102   //* start playback.
#define COMMAND_PAUSE           0x103   //* pause the playback.
#define COMMAND_STOP            0x104   //* stop the playback.
#define COMMAND_SEEKTO          0x105   //* seek to posion, in unit of second.
#define COMMAND_SHOW_MEDIAINFO  0x106   //* show media information.
#define COMMAND_SHOW_DURATION   0x107   //* show media duration, in unit of second.
#define COMMAND_SHOW_POSITION   0x108   //* show current play position, in unit of second.
#define COMMAND_SWITCH_AUDIO    0x109   //* switch autio track.
#define COMMAND_SETSPEED        0x10a


static void set_source(DemoPlayerContext *demoPlayer, char* pUrl)
{
    demoPlayer->mSeekable = 1;

    //* set url to the AwPlayer.
    if(XPlayerSetDataSourceUrl(demoPlayer->mAwPlayer,
                 (const char*)pUrl, NULL, NULL) != 0)
    {
        printf("error:\n");
        printf("    AwPlayer::setDataSource() return fail.\n");
        return;
    }
    printf("setDataSource end\n");

    if (!strncmp(pUrl, "http://", 7)) {
        if(XPlayerPrepareAsync(demoPlayer->mAwPlayer) != 0)
        {
            printf("error:\n");
            printf("    AwPlayer::prepareAsync() return fail.\n");
            pthread_mutex_unlock(&demoPlayer->mMutex);
            return;
        }
        sem_wait(&demoPlayer->mPrepared);
    }

	//* start preparing.
    pthread_mutex_lock(&demoPlayer->mMutex);
    demoPlayer->mPreStatus = STATUS_STOPPED;
    demoPlayer->mStatus    = STATUS_PREPARING;
    pthread_mutex_unlock(&demoPlayer->mMutex);
    printf("preparing...\n");
}

static void play(DemoPlayerContext *demoPlayer)
{
    if(XPlayerStart(demoPlayer->mAwPlayer) != 0)
    {
        printf("error:\n");
        printf("    AwPlayer::start() return fail.\n");
        return;
    }
    demoPlayer->mPreStatus = demoPlayer->mStatus;
    demoPlayer->mStatus    = STATUS_PLAYING;
    printf("playing.\n");
}

static void pause(DemoPlayerContext *demoPlayer)
{
    if(XPlayerPause(demoPlayer->mAwPlayer) != 0)
    {
        printf("error:\n");
        printf("    AwPlayer::pause() return fail.\n");
        return;
    }
    demoPlayer->mPreStatus = demoPlayer->mStatus;
    demoPlayer->mStatus    = STATUS_PAUSED;
    printf("paused.\n");
}

static void stop(DemoPlayerContext *demoPlayer)
{
    if(XPlayerReset(demoPlayer->mAwPlayer) != 0)
    {
        printf("error:\n");
        printf("    AwPlayer::reset() return fail.\n");
        return;
    }
    demoPlayer->mPreStatus = demoPlayer->mStatus;
    demoPlayer->mStatus    = STATUS_STOPPED;
    printf("stopped.\n");
}


//* a callback for awplayer.
static int CallbackForAwPlayer(void* pUserData, int msg, int ext1, void* param)
{
    DemoPlayerContext* pDemoPlayer = (DemoPlayerContext*)pUserData;

    switch(msg)
    {
        case AWPLAYER_MEDIA_INFO:
        {
            switch(ext1)
            {
                case AW_MEDIA_INFO_NOT_SEEKABLE:
                {
                    pDemoPlayer->mSeekable = 0;
                    printf("info: media source is unseekable.\n");
                    break;
                }
                case AW_MEDIA_INFO_RENDERING_START:
                {
                    printf("info: start to show pictures.\n");
                    break;
                }
            }
            break;
        }

        case AWPLAYER_MEDIA_ERROR:
        {
            pthread_mutex_lock(&pDemoPlayer->mMutex);
            pDemoPlayer->mStatus = STATUS_STOPPED;
            pDemoPlayer->mPreStatus = STATUS_STOPPED;
            pthread_mutex_unlock(&pDemoPlayer->mMutex);
            printf("error: open media source fail.\n");
            sem_post(&pDemoPlayer->mPrepared);
            sem_post(&pDemoPlayer->mStoped);
            pDemoPlayer->mError = 1;

            loge(" error : how to deal with it");
            break;
        }

        case AWPLAYER_MEDIA_PREPARED:
        {
            logd("info : preared");
            pDemoPlayer->mPreStatus = pDemoPlayer->mStatus;
            pDemoPlayer->mStatus = STATUS_PREPARED;
            sem_post(&pDemoPlayer->mPrepared);
            printf("info: prepare ok.\n");
            break;
        }

        case AWPLAYER_MEDIA_PLAYBACK_COMPLETE:
        {
            sem_post(&pDemoPlayer->mStoped);//* stop the player.
            //* TODO
            break;
        }

        case AWPLAYER_MEDIA_SEEK_COMPLETE:
        {
            pthread_mutex_lock(&pDemoPlayer->mMutex);
            pDemoPlayer->mStatus = pDemoPlayer->mPreStatus;
            printf("info: seek ok.\n");
            pthread_mutex_unlock(&pDemoPlayer->mMutex);
            break;
        }

        default:
        {
            //printf("warning: unknown callback from AwPlayer.\n");
            break;
        }
    }

    return 0;
}

static uint8_t cedarx_inited = 0;
static uint8_t xplayer_inited = 0;
void AwParserInit(void);
void AwStreamInit(void);

static DemoPlayerContext *demoPlayer;
static FATFS fs;

static enum cmd_status cmd_cedarx_create_exec(char *cmd)
{
    FRESULT res;
    demoPlayer = malloc(sizeof(*demoPlayer));

	if (cedarx_inited++ == 0) {
		printf_lock_init();
		if ((res = f_mount(&fs, "0:/", 1)) != FR_OK) {
			printf("can not mount\n");
			return -1;
		} else {
			printf("mount success\n");
		}
	}

    if (!xplayer_inited) {
        xplayer_inited = 1;
        AwParserInit();
        AwStreamInit();
    }

    //* create a player.
    memset(demoPlayer, 0, sizeof(DemoPlayerContext));
    pthread_mutex_init(&demoPlayer->mMutex, NULL);
    sem_init(&demoPlayer->mStoped, 0, 0);
    sem_init(&demoPlayer->mPrepared, 0, 0);

    demoPlayer->mAwPlayer = XPlayerCreate();
    if(demoPlayer->mAwPlayer == NULL)
    {
        printf("can not create AwPlayer, quit.\n");
        return -1;
    } else {
        printf("create AwPlayer success.\n");
    }

    //* set callback to player.
    XPlayerSetNotifyCallback(demoPlayer->mAwPlayer, CallbackForAwPlayer, (void*)demoPlayer);

    //* check if the player work.
    if(XPlayerInitCheck(demoPlayer->mAwPlayer) != 0)
    {
        printf("initCheck of the player fail, quit.\n");
        XPlayerDestroy(demoPlayer->mAwPlayer);
        demoPlayer->mAwPlayer = NULL;
        return -1;
    } else
        printf("AwPlayer check success.\n");

    SoundCtrl* sound = SoundDeviceCreate();

    XPlayerSetAudioSink(demoPlayer->mAwPlayer, (void*)sound);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_destroy_exec(char *cmd)
{
    FRESULT res;

    printf("destroy AwPlayer.\n");

    if(demoPlayer->mAwPlayer != NULL)
    {
        XPlayerDestroy(demoPlayer->mAwPlayer);
        demoPlayer->mAwPlayer = NULL;
    }

    printf("destroy AwPlayer 1.\n");
    pthread_mutex_destroy(&demoPlayer->mMutex);

    sem_destroy(&demoPlayer->mPrepared);
    sem_destroy(&demoPlayer->mStoped);

	if (--cedarx_inited == 0) {
		if ((res = f_mount(NULL, "", 1)) != FR_OK)
			printf("failed to unmount\n");
		printf_lock_deinit();
	}

	free(demoPlayer);

    return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_play_exec(char *cmd)
{
    play(demoPlayer);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_stop_exec(char *cmd)
{
    stop(demoPlayer);
    msleep(5);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_pause_exec(char *cmd)
{
    pause(demoPlayer);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_seturl_exec(char *cmd)
{
    set_source(demoPlayer, cmd);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_getpos_exec(char *cmd)
{
	int msec;
	XPlayerGetCurrentPosition(demoPlayer->mAwPlayer, &msec);
	cmd_write_respond(CMD_STATUS_OK, "played time: %d ms", msec);
	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_cedarx_seek_exec(char *cmd)
{
	int nSeekTimeMs = atoi(cmd);

	XPlayerSeekTo(demoPlayer->mAwPlayer, nSeekTimeMs);
	return CMD_STATUS_OK;
}

#include "driver/chip/hal_codec.h"
#include "audio/manager/audio_manager.h"

static enum cmd_status cmd_cedarx_setvol_exec(char *cmd)
{
	int vol = atoi(cmd);
	if (vol > 31)
		vol = 31;

	aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, vol);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_playdic_exec(char *cmd)
{
	return CMD_STATUS_OK;
}


#include <CdxQueue.h>
#include <AwPool.h>
#include <CdxMuxer.h>
#include "awencoder.h"
#include "RecoderWriter.h"
#include "xrecord.h"
#include "CaptureControl.h"


static uint8_t xrecord_inited = 0;
static XRecord *xrecord;

static enum cmd_status cmd_cedarx_rec_exec(char *cmd)
{
		FRESULT res;

		XRECODER_AUDIO_ENCODE_TYPE type;
		if (strstr(cmd, ".amr"))
			type = XRECODER_AUDIO_ENCODE_AMR_TYPE;
		else if (strstr(cmd, ".pcm"))
			type = XRECODER_AUDIO_ENCODE_PCM_TYPE;
		else {
			printf("do not support this encode type\n");
			return CMD_STATUS_INVALID_ARG;
		}

		if (cedarx_inited++ == 0) {
			printf_lock_init();

			if ((res = f_mount(&fs, "0:/", 1)) != FR_OK) {
				printf("can not mount\n");
				return -1;
			}
		}


	void AwMuxerInit();
		if (xrecord_inited == 0) {
			AwMuxerInit();
			xrecord_inited = 1;
		}

		xrecord = XRecordCreate();
		if (xrecord == NULL)
			printf("create success\n");

		CaptureCtrl* cap = CaptureDeviceCreate();
		if (!cap)
			printf("cap device create failed\n");
		XRecordSetAudioCap(xrecord, cap);
				AudioEncodeConfig audioConfig;

				audioConfig.nType = AUDIO_ENCODE_AMR_TYPE;
				audioConfig.nInChan = 1;
				audioConfig.nInSamplerate = 8000;
				audioConfig.nOutChan = 1;
				audioConfig.nOutSamplerate = 8000;
				audioConfig.nSamplerBits = 16;
				audioConfig.nBitrate = 12200;
				audioConfig.nFrameStyle = 0;


		XRecordSetDataDstUrl(xrecord, cmd, NULL, NULL);
		XRecordSetAudioEncodeType(xrecord, type, &audioConfig);

		XRecordPrepare(xrecord);
		XRecordStart(xrecord);
		printf("record start\n");


	return CMD_STATUS_OK;
}

static enum cmd_status cmd_cedarx_end_exec(char *cmd)
{
	FRESULT res;

    XRecordStop(xrecord);
	printf("record stop\n");
	XRecordDestroy(xrecord);
	printf("record destroy\n");

	if (--cedarx_inited == 0) {
		printf_lock_deinit();

		if ((res = f_mount(NULL, "", 1)) != FR_OK)
			printf("failed to unmount\n");
	}

	return CMD_STATUS_OK;
}


/*
 * brief cedarx Test Command
 * command  cedarx create
 *          cedarx destroy
 *          cedarx play
 *          cedarx stop
 *          cedarx pause
 *          cedarx seturl file://music/01.mp3
 *			cedarx getpos
 *			cedarx seek 6000
 *			cedarx setvol 8
 *          cedarx playdic file://music
 *			cedarx rec file://record/wechat.amr
 *			cedarx end
 */
static struct cmd_data g_cedarx_cmds[] = {
    { "create",     cmd_cedarx_create_exec    },
    { "destroy",    cmd_cedarx_destroy_exec     },
    { "play",       cmd_cedarx_play_exec    },
    { "stop",       cmd_cedarx_stop_exec    },
    { "pause",      cmd_cedarx_pause_exec     },
    { "seturl",     cmd_cedarx_seturl_exec     },
    { "getpos",     cmd_cedarx_getpos_exec     },
    { "seek",      	cmd_cedarx_seek_exec     },
    { "setvol",     cmd_cedarx_setvol_exec     },
    { "playdic",    cmd_cedarx_playdic_exec    },

	{ "rec",     cmd_cedarx_rec_exec    },
	{ "end",     cmd_cedarx_end_exec    },
};

enum cmd_status cmd_cedarx_exec(char *cmd)
{
    return cmd_exec(cmd, g_cedarx_cmds, cmd_nitems(g_cedarx_cmds));
}

#endif /* __PRJ_CONFIG_XPLAYER */
