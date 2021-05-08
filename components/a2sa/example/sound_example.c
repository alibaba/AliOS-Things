/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

extern int audio_install_codec_driver();
extern void sound_example_wav_entry(int argc, char **argv);
extern void sound_example_loopback_entry(int argc, char **argv);
void sound_example_setvol_entry(int argc, char **argv);
void sound_example_getvol_entry(int argc, char **argv);

static void sound_example_install_driver(int argc, char **argv)
{
    printf("sound install driver test begin ...\r\n");
    audio_install_codec_driver();
    printf("sound install driver test end !!!\r\n");
    return;
}

static void sound_example_wav(int argc, char **argv)
{
    printf("sound wav player test begin ...\r\n");
    sound_example_wav_entry(argc, argv);
    printf("sound wav player test end !!!\r\n");
    return;
}

static void sound_example_loopback(int argc, char **argv)
{
    sound_example_loopback_entry(argc, argv);
    return;
}

static void sound_example_setvol(int argc, char **argv)
{
    printf("sound setvol test begin ...\r\n");
    sound_example_setvol_entry(argc, argv);
    printf("sound setvol test end !!!\r\n");
    return;
}

static void sound_example_getvol(int argc, char **argv)
{
    printf("sound getvol test begin ...\r\n");
    sound_example_getvol_entry(argc, argv);
    printf("sound getvol test end !!!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(sound_example_install_driver, sound_install_driver, sound install driver test example)
ALIOS_CLI_CMD_REGISTER(sound_example_wav, sound_wav, sound wav player test example)
ALIOS_CLI_CMD_REGISTER(sound_example_loopback, sound_loopback, sound loopback test example)
ALIOS_CLI_CMD_REGISTER(sound_example_setvol, sound_setvol, sound set volume test example)
ALIOS_CLI_CMD_REGISTER(sound_example_getvol, sound_getvol, sound get volume test example)
#endif
