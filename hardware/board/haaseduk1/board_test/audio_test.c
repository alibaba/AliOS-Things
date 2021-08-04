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
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"
#include <aos/kernel.h>

#include "uvoice_init.h"
#include "uvoice_test.h"
aos_task_t g_audio_test_task;
static char *param_array[2];
static int audio_loopback_flag = 1;
static uvoice_player_t *uvocplayer;
extern int audio_install_codec_driver();

void audio_test_stop(int item)
{
    if (item == 0) {
        char *param_array[2];
        param_array[0] = "play";
        param_array[1] = "stop";
        uvoice_play_test(2, param_array);
        param_array[0] = "play";
        param_array[1] = "clear";
        uvoice_play_test(2, param_array);
    } else {
        param_array[0] = "loopback";
        param_array[1] = "stop";
        sound_example_loopback_entry(2, param_array);
    }
}

int audio_test_speeker(void)
{
    param_array[0] = "play";
    param_array[1] = "stopsync";
    uvoice_play_test(2, param_array);

    param_array[0] = "play";
    param_array[1] = "clear";
    uvoice_play_test(2, param_array);

    param_array[0] = "play";
    param_array[1] = "fs:/data/mp3/teststart.mp3";
    uvoice_play_test(2, param_array);

    return 0;
}

void cmd_audiotest_handler(char *buf, int len, int argc, char **argv)
{
    char *param_array[2];
    if (argc < 2 || strcmp(argv[0], "play"))
        return;

    if (!strcmp(argv[1], "pause")) {
        param_array[0] = "play";
        param_array[1] = "pause";
        uvoice_play_test(2, param_array);
    } else if (!strcmp(argv[1], "resume")) {
        param_array[0] = "play";
        param_array[1] = "resume";
        uvoice_play_test(2, param_array);
    } else if (!strcmp(argv[1], "loopback")) {
        param_array[0] = "play";
        param_array[1] = "loopback";
        uvoice_play_test(2, param_array);
    } else if (!strcmp(argv[1], "stop")) {
        param_array[0] = "play";
        param_array[1] = "stop";
        uvoice_play_test(2, param_array);
        param_array[0] = "play";
        param_array[1] = "clear";
        uvoice_play_test(2, param_array);
    } else {
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
}

struct cli_command audio_test_commands[] = {
    {"play", "try 'play fs:/data/mp3/teststart.mp3' | 'play url' | play pause | play pause | play resume", cmd_audiotest_handler}
};

int audio_test(int item)
{
    if (item == 0) {
        uvoice_init();
        audio_install_codec_driver();
        aos_set_master_volume(100);
        aos_cli_register_commands(audio_test_commands,
                                  sizeof(audio_test_commands) / sizeof(struct cli_command));
        aos_task_create(&g_audio_test_task, "audio_test_speeker", audio_test_speeker, NULL, NULL, 4096, 30, 0x01u);

    } else if (item == 1) {
        printf("\r\n\r\n");
        printf("****************************************************************\r\n");
        printf("************************* Audio Test ***************************\r\n");
        printf("****************************************************************\r\n");
        printf("*How to test: please speaker any words, then you will hear it **\r\n");
        printf("****************************************************************\r\n");
        printf("===== AUDIO test : Start=====\r\n");
        param_array[0] = "loopback";
        param_array[1] = "start";
        sound_example_loopback_entry(2, param_array);
    }
}
