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

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

//#include "uvoice_os.h"

#include "uvoice_init.h"
#include "uvoice_test.h"

static int audio_loopback_flag = 1;
static uvoice_player_t *uvocplayer;
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

void audio_test_stop(int item)
{
    char *param_array[3];
    if(item == 0){
        audio_loopback_flag = 0;
        //aos_task_delete("audio_test_speeker");
		uvocplayer->set_fade(40, 40);
		uvocplayer->stop_async();
    }else{
        param_array[0] = "loopback";
        param_array[1] = "0";
        param_array[2] = "stop";
        cmd_sound_loop_handler(NULL, 0 ,3, param_array);
    }
}

static void cmd_record_handler2(char *buf, int len, int argc, char **argv)
{
    char *param_array[3];
    param_array[0] = "record";
	param_array[1] = "start";
    param_array[2] = "1";
	return uvoice_record_test(argc, argv);
}

struct cli_command uvoicedemo_commands[] = {
	{"play", "player test", cmd_play_handler},
	{"record", "record test", cmd_record_handler},
	{"msleep", "sleep millisecond", cmd_msleep_handler},
	{"freemem", "free memory size", cmd_freemem_handler},
};

int audio_test_speeker(void)
{
    uvocplayer = uvoice_player_create();
    uvocplayer->set_volume(10);
    while (audio_loopback_flag)
    {
        uvocplayer->playback("fs:/data/1.mp3");
        aos_msleep(5000);
    }
    return 0;
}

int audio_test_loopback(void)
{
    FILE *ifp;
    printf("\r\n\r\n");
    printf("****************************************************************\r\n");
    printf("************************* Audio Test ***************************\r\n");
    printf("****************************************************************\r\n");
    printf("*How to test: please speaker any words, then you will hear it **\r\n");
    printf("****************************************************************\r\n");
    printf("===== AUDIO test : Start=====\r\n");
#if 1
 char *param_array[7];
    param_array[0] = "loopback";
    param_array[1] = "0";
    param_array[2] = "start";
    cmd_sound_loop_handler(NULL, 0 ,3, param_array);
#endif
    return 0;
}

int audio_test(int item)
{
	uvoice_init();
    audio_install_codec_driver();
    //aos_set_master_volume(100);

	aos_cli_register_commands(uvoicedemo_commands,
		sizeof(uvoicedemo_commands) / sizeof(struct cli_command));
    //printf("enter audio_test item = %d\n",item);
    if(item == 0)
        aos_task_new("audio_test_speeker", audio_test_speeker, NULL, 4096);
    else if(item == 1)
        return audio_test_loopback();

}