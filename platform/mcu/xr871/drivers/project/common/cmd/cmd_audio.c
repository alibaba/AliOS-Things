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

#include "cmd_util.h"
#include "cmd_audio.h"
#include "audio/pcm/audio_pcm.h"
#include "audio/manager/audio_manager.h"
#include "driver/chip/hal_codec.h"
#include "fs/fatfs/ff.h"
#include "common/framework/fs_ctrl.h"

#define SOUND_PLAYCARD			AUDIO_CARD0
#define SOUND_CAPCARD			AUDIO_CARD0

#define TEST_DELAY_TIME			0X1FF
#define SR_NUM					(9)

struct pcm_config audio_pcm_config = {
        .channels = 2,
        .rate = 44100,
        .period_size = 2048,
        .period_count = 2,
        .format = PCM_FORMAT_S16_LE,
};

static uint32_t sampleRate[SR_NUM] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};

#define SEARCH_FOREACH_CONFIG(a, b, c, i) 	for (i = 0; i < SR_NUM; i++) { \
							if (b !=1 && b!=2) 							   \
								return;									   \
							if (sampleRate[i] == a) {					   \
								audio_pcm_config.rate = a;				   \
								audio_pcm_config.channels = b;			   \
								c = &audio_pcm_config; 					   \
								break; 									   \
							} 											   \
						}

#define CREATE_CAP_FILE(FILE_PATH, RES, FILE) \
	FRESULT RES; \
	FIL file; \
	do { \
		if (fs_ctrl_mount(FS_MNT_DEV_TYPE_SDCARD, 0) != 0) { \
			CMD_ERR("mount fail\n"); \
			RES = FR_DISK_ERR; \
		} else if ((RES = f_open(&file, FILE_PATH, FA_OPEN_ALWAYS|FA_READ|FA_WRITE)) != FR_OK) { \
			CMD_ERR("[music file]failed to open,%s\n", FILE_PATH); \
		} \
	} while (0)

#define AUDIO_THREAD_STACK_SIZE		(2 * 1024)
static OS_Thread_t g_audio_stream_thread;
static OS_Thread_t g_audio_control_thread;
static uint8_t g_audio_task_end;

#define AUDIO_DELETE_THREAD(THREAD)  OS_ThreadDelete(&THREAD)

#define AUDIO_CREAT_THREAD(THREAD, TASK, ARG)         \
                        if (OS_ThreadIsValid(&THREAD)) { \
                                CMD_ERR("audio task is running\n"); \
                                return CMD_STATUS_FAIL; \
                        } \
                        if (OS_ThreadCreate(&THREAD, \
                                   "",               \
                                   TASK,    \
                                   (void *)ARG,         \
                                   OS_THREAD_PRIO_APP,   \
                                   AUDIO_THREAD_STACK_SIZE) != OS_OK) { \
                                CMD_ERR("audio task create failed\n"); \
                                return CMD_STATUS_FAIL; \
                       }

void cap_exec(void *cmd)
{
	int argc, i = 0, ret = 0;
	unsigned int writenum = 0;
	char *argv[5];
	struct pcm_config *config = NULL;

	argc = cmd_parse_argv(cmd, argv, 5);
	if (argc < 3) {
		CMD_ERR("invalid audio capture cmd, argc %d\n", argc);
		goto exit_thread;
	}
	int samplerate = cmd_atoi(argv[0]);
	int channels = cmd_atoi(argv[1]);

	CMD_DBG("CMD:drv audio cap (samplerate)%d (channel)%d (file)%s\n", samplerate, channels, argv[2]);

	f_unlink(argv[2]);
	CREATE_CAP_FILE(argv[2], result, file);
	if (result != FR_OK) {
		CMD_ERR("creat file failed: %d.\n", result);
		goto exit_thread;
	}

	SEARCH_FOREACH_CONFIG(samplerate, channels, config, i);
	if (config == NULL) {
		CMD_ERR("invalid audio cap param.\n");
		f_close(&file);
		goto exit_thread;
	}

    unsigned int pcm_buf_size = (config->channels)*2*(config->period_size);
    char *pcm_data = malloc(pcm_buf_size);
    if (pcm_data == NULL) {
		CMD_ERR("malloc buf failed\n");
		f_close(&file);
		goto exit_thread;
    }
    memset(pcm_data, 0, pcm_buf_size);

    if (snd_pcm_open(config, SOUND_CAPCARD, PCM_IN) != 0)
    {
		CMD_ERR("sound card open err\n");
		goto exit;
    }
	unsigned int delay_time = TEST_DELAY_TIME;

	g_audio_task_end = 0;
	CMD_DBG("Capture run.\n");
    while (!g_audio_task_end && --delay_time != 0) {
            ret = snd_pcm_read(config, SOUND_CAPCARD, pcm_data, pcm_buf_size);
            if (ret != pcm_buf_size) {
				CMD_ERR("read data failed(%d), line:%d\n", ret, __LINE__);
				break;
            }
            if ((result = f_write(&file, pcm_data, pcm_buf_size, &writenum)) != FR_OK) {
				CMD_ERR("write failed(%d).\n",result);
				break;
            } else {
				if (writenum != pcm_buf_size) {
					CMD_ERR("write failed %d,%d\n", writenum, __LINE__);
					break;
				}
            }
    }
    snd_pcm_close(SOUND_CAPCARD, PCM_IN);

exit:
	free(pcm_data);
	f_close(&file);
	CMD_DBG("Capture end.\n");
exit_thread:
	AUDIO_DELETE_THREAD(g_audio_stream_thread);
}

void play_exec(void *cmd)
{
	int argc, i = 0;
	unsigned int readnum = 0;
	char *argv[5];
	struct pcm_config *config = NULL;

	argc = cmd_parse_argv(cmd, argv, 5);
	if (argc < 3) {
		CMD_ERR("invalid audio capture cmd, argc %d\n", argc);
		goto exit_thread;
	}
	int samplerate = cmd_atoi(argv[0]);
	int channels = cmd_atoi(argv[1]);

	CMD_DBG("CMD:drv audio play (samplerate)%d (channel)%d (file)%s\n", samplerate, channels,argv[2]);

	CREATE_CAP_FILE(argv[2], result, file);
	if (result != FR_OK) {
		CMD_ERR("creat file failed: %d.\n", result);
		goto exit_thread;
	}

	SEARCH_FOREACH_CONFIG(samplerate, channels, config, i);
	if (config == NULL) {
		CMD_ERR("invalid audio cap param.\n");
		f_close(&file);
		goto exit_thread;
	}

    unsigned int pcm_buf_size = (config->channels)*2*(config->period_size);
    char *pcm_data = malloc(pcm_buf_size);
    if (pcm_data == NULL) {
		CMD_ERR("malloc buf failed\n");
		f_close(&file);
		goto exit_thread;;
    }
    if (snd_pcm_open(config, SOUND_PLAYCARD, PCM_OUT) != 0)
    {
		CMD_ERR("sound card open err\n");
		goto exit;
    }
	CMD_DBG("Play on.\n");
	g_audio_task_end = 0;
    while (1 && !g_audio_task_end) {
	    if ((result = f_read(&file, pcm_data, pcm_buf_size, &readnum)) != FR_OK) {
	        CMD_ERR("read failed(%d).\n",result);
			break;
	    } else {
            if (readnum != pcm_buf_size) {
				CMD_DBG("file end: file size = %d\n", readnum);
				break;
            }
		}
		snd_pcm_write(config, SOUND_PLAYCARD, pcm_data, pcm_buf_size);
    }
    snd_pcm_close(SOUND_PLAYCARD, PCM_OUT);

exit:
	f_close(&file);
	free(pcm_data);
	CMD_DBG("Play end.\n");
exit_thread:
	AUDIO_DELETE_THREAD(g_audio_stream_thread);
}

void vol_exec(void *cmd)
{
	int argc;
	char *argv[3];

	argc = cmd_parse_argv(cmd, argv, 3);
	if (argc < 2) {
		CMD_ERR("invalid audio set vol cmd, argc %d\n", argc);
		goto exit;
	}

	uint16_t dev = cmd_atoi(argv[0]);
	int vol = cmd_atoi(argv[1]);
	CMD_DBG("CMD:drv audio vol (level)%d\n", vol);

	if ( vol > aud_mgr_maxvol()) {
		CMD_ERR("invalid audio vol.Range(0-31)\n");
		goto exit;
	}

	aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, dev, vol);
exit:
	AUDIO_DELETE_THREAD(g_audio_control_thread);
}

void path_exec(void *cmd)
{
	int argc;
	char *argv[2];

	argc = cmd_parse_argv(cmd, argv, 2);
	if (argc < 1) {
		CMD_ERR("invalid audio set path cmd, argc %d\n", argc);
		goto exit;
	}
	int path = cmd_atoi(argv[0]);
	CMD_DBG("CMD:drv audio out-path %d\n", path);

	if (path > AUDIO_OUT_DEV_SPEAKER) {
		CMD_ERR("invalid audio out-path.Range(1-2)\n");
		goto exit;
	}

	aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, path, cmd_atoi(argv[1]));
exit:
	AUDIO_DELETE_THREAD(g_audio_control_thread);
}

static enum cmd_status audio_play_task(char *arg)
{
	char *cmd = (char *)arg;

	AUDIO_CREAT_THREAD(g_audio_stream_thread, play_exec, cmd);
	return CMD_STATUS_OK;
}

static enum cmd_status audio_cap_task(char *arg)
{
	char *cmd = (char *)arg;

	AUDIO_CREAT_THREAD(g_audio_stream_thread, cap_exec, cmd);
	return CMD_STATUS_OK;
}

static enum cmd_status audio_vol_task(char *arg)
{
	char *cmd = (char *)arg;

	AUDIO_CREAT_THREAD(g_audio_control_thread, vol_exec, cmd);
	return CMD_STATUS_OK;
}

static enum cmd_status audio_path_task(char *arg)
{
	char *cmd = (char *)arg;

	AUDIO_CREAT_THREAD(g_audio_control_thread, path_exec, cmd);
	return CMD_STATUS_OK;
}

static enum cmd_status audio_end_task(char *arg)
{
	char *cmd = (char *)arg;
	(void)cmd;

    if (OS_ThreadIsValid(&g_audio_stream_thread)) {
		g_audio_task_end = 1;
		CMD_DBG("audio task end\n");
	}

	return CMD_STATUS_OK;
}

/*
 * brief audio Test Command
 * command
 *		1)cap:	$ audio cap [samplerate] [channels] [file-path]
 *		2)play:	$ audio play [samplerate] [channels] [file-path]
 *		3)vol:	$ audio vol [dev] [vol]
 *		4)path:	$ audio path [dev] [en]
 *		5)end : $ audio end
 *		samplerate: [8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000]
 *		channels:   [1~2]
 *		vol:   	    [0~31]
 *		dev:		device mask
 *		en:			[0~1]
 * example
 *		audio cap 16000 1 record.pcm
 *		audio play 44100 2 music.pcm
 *      audio vol 12
 *		audio path	1 1
 *		audio end
 */

static struct cmd_data g_audio_cmds[] = {
	{ "cap",     audio_cap_task },
	{ "play",    audio_play_task },
	{ "vol",     audio_vol_task },
	{ "path",    audio_path_task },
	{ "end",     audio_end_task },
};

void audio_task_run(void *arg)
{
	char *cmd = (char *)arg;
	cmd_exec(cmd, g_audio_cmds, cmd_nitems(g_audio_cmds));
}

enum cmd_status cmd_audio_exec(char *cmd)
{
	audio_task_run(cmd);
	return CMD_STATUS_OK;
}
