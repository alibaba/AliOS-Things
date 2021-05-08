/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 *
 */
#include "k_api.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "ulog/ulog.h"
#include "uvoice_init.h"
#include "uvoice_test.h"

void cmd_tts_handler(char *buf, int len, int argc, char **argv)
{
    /* >> tts "我爱你中国" /data/tts.mp3 */
    extern void test_tts_handle(int argc, char **argv);
    return test_tts_handle(argc, argv);
}

void cmd_play_handler(char *buf, int len, int argc, char **argv)
{
    /* >> play fs:/data/6.mp3 */
	char *param_array[2];
	if (argc < 2 || strcmp(argv[0], "play"))
		return;

    if(!strcmp(argv[1], "pause")) {
        param_array[0] = "play";
        param_array[1] = "pause";
        uvoice_play_test(2, param_array);
    } else if(!strcmp(argv[1], "resume")) {
        param_array[0] = "play";
        param_array[1] = "resume";
        uvoice_play_test(2, param_array);
    } else if(!strcmp(argv[1], "stop")) {
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

#if AOS_COMP_CLI
struct cli_command audio_demo_commands[] = {
	{"play", "try 'play fs:/data/6.mp3' | 'play url' | play pause | play pause | play resume", cmd_play_handler},
	{"tts", "try 'tts 我爱你中国 /data/tts.mp3", cmd_tts_handler},
};
#endif /* AOS_COMP_CLI */

int haas_audio_init(void)
{
	uvoice_init();
    #if AOS_COMP_CLI
	aos_cli_register_commands(audio_demo_commands, sizeof(audio_demo_commands) / sizeof(struct cli_command));
    #endif /* AOS_COMP_CLI */
    return 0;
}
