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
#include "uvoice_tts.h"

#include "../internal/uvoice_os.h"


#define APP_LOGI(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)

static os_task_t voicelouder_task;

static char tts_text[128];
static char tts_filename[128];
static bool tts_configred = false;

static int alicloud_tts_config(void)
{
    tts_config_t tts_config;
	uvoice_tts_t *tts;
	int ret;

    /** 请开发者根据个人在智能语音交互平台的账号信息，更新下面的app_key和token
     *   https://nls-portal.console.aliyun.com/overview
     **/
    tts_config.app_key = "NULL";
    tts_config.token   = "NULL";
    tts_config.format = MEDIA_FMT_MP3;
    tts_config.sample_rate = 16000;
    tts_config.voice = "siqi";
    tts_config.volume = 80;
    tts_config.speech_rate = 0;
    tts_config.pitch_rate = 0;
    tts_config.text_encode_type = TTS_ENCODE_UTF8;

    if(0 == strcmp(tts_config.app_key, "NULL")) {
        printf("aliyun tts need app key and token !!!\r\n");
        printf("help document: https://help.aliyun.com/document_detail/72153.html\n");
        return -1;
    }

	tts = uvoice_tts_create();
    if (!tts) {
        APP_LOGE("create tts failed !\n");
        return -1;
    }

    if (tts->tts_init(TTS_AICLOUD_ALIYUN, &tts_config)) {
        APP_LOGE("config tts failed !\n");
		uvoice_tts_release(tts);
        return -1;
    }
    APP_LOGI("config tts success\n");

    return 0;
}

static int alicoud_tts_event(tts_event_e event, char *info)
{
	return 0;
}

static int alicloud_tts_recv_data(uint8_t *buffer, int nbytes, int index)
{
    FILE *fp;
    if (index == 0) {
        fp = fopen(tts_filename, "w");
        if (!fp) {
            printf("%s: open %s failed !\n", __func__, tts_filename);
            return -1;
        }
    } else {
        fp = fopen(tts_filename, "a+");
        if (!fp) {
            printf("%s: open %s failed !\n", __func__, tts_filename);
            return -1;
        }
    }
    printf("write data %d size %d to file %s\n",
		index, nbytes, tts_filename);
    fwrite(buffer, nbytes, 1, fp);
    fclose(fp);
	return 0;
}

static void alicloud_tts_download_task(void *arg)
{
	tts_recv_callback_t recv_cb;
	uvoice_tts_t *tts;
    int ret = 0;

	if (!tts_configred) {
		ret = alicloud_tts_config();
        if(ret != 0) {
            printf("tts download %s fail\n", tts_filename);
            return;
        }
		tts_configred = true;
	}

	tts = uvoice_tts_create();
    if (!tts) {
        APP_LOGE("create tts failed !\n");
        return;
    }

    recv_cb.event = alicoud_tts_event;
    recv_cb.recv_data = alicloud_tts_recv_data;

    ret = tts->tts_request(tts_text, TTS_RECV_DATA, &recv_cb);//uvoice_tts_aliyun_request()
    if (ret != 0) {
        printf("tts failed\n");
        return;
    }

    printf("tts download %s success\n", tts_filename);
}

int alicloud_tts_download(char *text, char *filename)
{
	if (!text || !filename) {
		APP_LOGE("args null !\n");
		return -1;
	}

    memset(tts_text, 0, sizeof(tts_text));
    snprintf(tts_text, sizeof(tts_text), "%s", text);

    memset(tts_filename, 0, sizeof(tts_filename));
    snprintf(tts_filename, sizeof(tts_filename), "%s", filename);

    os_task_create(&voicelouder_task, "tts_download_task",
		alicloud_tts_download_task,
		NULL, 8192*2, UVOICE_TASK_PRI_NORMAL);
	return 0;
}

void test_tts_handle(int argc, char **argv)
{
	if (argc < 3)
		return;
	alicloud_tts_download(argv[1], argv[2]);
}
