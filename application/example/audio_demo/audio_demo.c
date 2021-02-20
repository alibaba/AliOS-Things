/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "k_api.h"
#include "aos/cli.h"
#include "aos/yloop.h"
#include <aos/list.h>
#include "ulog/ulog.h"

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include "uvoice_init.h"
#include "uvoice_test.h"

#define APP_LOGI(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)

#define LOG_TAG "audio_demo"
typedef struct {
    struct dlist_s list;
    char *url;
	char *name;
	char *artist;
} playlist_item_t;

AOS_DLIST_HEAD(play_list);
static playlist_item_t *cur_item = NULL;

extern int audio_install_codec_driver();
extern void cmd_play_wav_handler(char *pbuffer, int outlen, int argc, char **argv);
extern void get_master_volume(char *pbuffer, int outlen, int argc, char **argv);
extern void set_master_volume(char *pbuffer, int outlen, int argc, char **argv);
extern void cmd_sound_loop_handler(char *pbuffer, int outlen, int argc, char **argv);

static void cmd_play_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_play_test(argc, argv);
}

static void cmd_record_handler(char *buf, int len, int argc, char **argv)
{
	return uvoice_record_test(argc, argv);
}

static void cmd_msleep_handler(char *buf, int len, int argc, char **argv)
{
	if (argc < 2)
		return;
	aos_msleep(atoi(argv[1]));
}

static void cmd_freemem_handler(char *buf, int len, int argc, char **argv)
{
	printf("freemem %d\n", g_kmm_head->free_size);
}

static void cmd_tts_handler(char *buf, int len, int argc, char **argv)
{
    extern void test_tts_handle(int argc, char **argv);
    return test_tts_handle(argc, argv);
}

void cmd_play_url_handler(char *buf, int len, int argc, char **argv)
{
	char *param_array[2];
	if (argc < 2 || strcmp(argv[0], "play_url"))
		return;
    
	LOGD(LOG_TAG, "%s:%d, %s %s", __func__, __LINE__, argv[0], argv[0]);
	param_array[0] = "play";
    param_array[1] = "stopsync";
    uvoice_play_test(2, param_array);

    param_array[0] = "play";
    param_array[1] = "clear";
    uvoice_play_test(2, param_array);

    param_array[0] = "play";
    param_array[1] = argv[1];
    uvoice_play_test(2, param_array);
}
        

void add_url_2_playlist(char *url)
{
	dlist_t *temp = NULL;
	playlist_item_t *iter = NULL;
	playlist_item_t *songItem = NULL;

	dlist_for_each_entry_safe(&play_list, temp, iter, playlist_item_t, list) {
        if(iter && !strcmp(iter->url, url)) {
			LOGE(LOG_TAG, "%s:%d, url %s exist.", __func__, __LINE__, url);
            return;
        }
	}

	songItem = malloc(sizeof(playlist_item_t));
	if(!songItem) {
        LOGE(LOG_TAG, "%s:%d, malloc playlist_item_t failed", __func__, __LINE__);
        return;
    }
	memset(songItem, 0, sizeof(playlist_item_t));
    songItem->url = strdup(url);
    if(!songItem->url) {
        LOGE(LOG_TAG, "%s:%d, malloc %d bytes failed", __func__, __LINE__, sizeof(playlist_item_t));
        return;
    }
    dlist_init(&songItem->list);
    dlist_add_tail(&songItem->list, &play_list);
	LOGD(LOG_TAG, "%s:%d, index %d, add url: %s", __func__, __LINE__, dlist_entry_number(&play_list), url);
}

/* externed to linkkit */
void cmd_add_driver_handler(char *pbuffer, int outlen, int argc, char **argv)
{
    if (argc < 1 || strcmp(argv[0], "insmod_audio_drv")) {
        printf("Usage: %s \r\n", argv[0]);
        return;
    }
    audio_install_codec_driver();
    return 0;
}

void cmd_add_url_handler(char *buf, int len, int argc, char **argv)
{
	if (argc < 2 || strcmp(argv[0], "add_url"))
		return;
	add_url_2_playlist(argv[1]);
}

void cmd_play_current_handler(char *buf, int len, int argc, char **argv)
{
	char *param_array[2];
	if (argc < 1 || strcmp(argv[0], "play_current"))
		return;
	if(!cur_item) {
		cur_item = dlist_first_entry(&play_list, playlist_item_t, list);
	}
	if(!cur_item || (&cur_item->list == &play_list) || !cur_item->url) {
		LOGE(LOG_TAG, "%s:%d, play list empty", __func__, __LINE__);
		cur_item = NULL;
		return;
	}
	LOGD(LOG_TAG, "%s:%d, url: %s", __func__, __LINE__, cur_item->url);
    param_array[0] = "play";
    param_array[1] = cur_item->url;
    uvoice_play_test(2, param_array);
}

void cmd_play_next_handler(char *buf, int len, int argc, char **argv)
{
	char *param_array[2];
	dlist_t *pList = NULL;
    playlist_item_t *pItem = NULL;
	if (argc < 1 || strcmp(argv[0], "play_next"))
		return;
	if(!cur_item) {
		pItem = dlist_first_entry(&play_list, playlist_item_t, list);
	} else {
		dlist_for_each_entry_safe(&cur_item->list, pList, pItem, playlist_item_t, list) {
            if(pItem && pItem->url && (pList->prev != &play_list)) {
                break;
            }
        }
	}
    cur_item = pItem;
	if(!cur_item || (&cur_item->list == &play_list) || !cur_item->url) {
		LOGE(LOG_TAG, "%s:%d, play list empty", __func__, __LINE__);
		return;
	}
	LOGD(LOG_TAG, "%s:%d, url: %s", __func__, __LINE__, cur_item->url);
	param_array[0] = "play";
    param_array[1] = cur_item->url;
	uvoice_play_test(2, param_array);
}

void cmd_play_pre_handler(char *buf, int len, int argc, char **argv)
{
	char *param_array[2];
    playlist_item_t *pItem = NULL;
	if (argc < 1 || strcmp(argv[0], "play_pre"))
		return;
	if(!cur_item) {
		pItem = dlist_first_entry(&play_list, playlist_item_t, list);
	} else {
		dlist_for_each_entry_reverse(pItem, &cur_item->list, list, playlist_item_t) {
            if(pItem && pItem->url && (&pItem->list != &play_list)) {
                break;
            }
        }
	}
	cur_item = pItem;
	if(!cur_item || (&cur_item->list == &play_list) || !cur_item->url) {
		LOGE(LOG_TAG, "%s:%d, play list empty", __func__, __LINE__);
		return;
	}
	LOGD(LOG_TAG, "%s:%d, url: %s", __func__, __LINE__, cur_item->url);
	param_array[0] = "play";
    param_array[1] = cur_item->url;
	uvoice_play_test(2, param_array);
}

struct cli_command uvoicedemo_commands[] = {
	{"play", "player test", cmd_play_handler},
	{"record", "record test", cmd_record_handler},
	{"tts", "tts text filepath", cmd_tts_handler},
	{"msleep", "sleep millisecond", cmd_msleep_handler},
	{"freemem", "free memory size", cmd_freemem_handler},
	{"insmod_audio_drv", "insmod a audio driver hal", cmd_add_driver_handler},
	{"play_wav", "play_wav /data/7.wav 10", cmd_play_wav_handler},
	{"loopback", "loopback 0/1/2 start/stop", cmd_sound_loop_handler},
	{"set_vol", "set volume xx (0~100)", set_master_volume},
	{"get_vol", "get_vol", get_master_volume},
	{"add_url", "add_url ", cmd_add_url_handler},
	{"play_current", "play_current ", cmd_play_current_handler},
	{"play_url", "play_url fs:/data/6.mp3 ", cmd_play_url_handler},
	{"play_next", "play_next ", cmd_play_next_handler},
	{"play_pre", "play_pre ", cmd_play_pre_handler},
};

int haas_audio_start(void)
{
	uvoice_init();

	aos_cli_register_commands(uvoicedemo_commands,
		sizeof(uvoicedemo_commands) / sizeof(struct cli_command));

    return 0;
}

