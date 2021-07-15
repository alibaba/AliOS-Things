#include "kws.h"
#include "../menu.h"
#include "aiagent_service.h"
#include "aiagent_common.h"
#include "beeper/beeper.h"

#define TAG "kws"
#include "uvoice_init.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"

MENU_COVER_TYP kws_cover = {MENU_COVER_NONE};
MENU_TASK_TYP  kws_tasks = {kws_init, kws_uninit};
MENU_TYP kws = {"kws", &kws_cover, &kws_tasks, NULL, NULL};

static os_task_t play_task;
static uvoice_player_t *uvocplayer;
static char text_source[256];
static int source_sample_rate;
static int source_channels;
static bool running = false;

extern int audio_install_codec_driver();

static int get_format_from_name(char *name, media_format_t *format)
{
	if (!name || !format) {
		LOGE(TAG, "arg null !\n");
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

static void *play_music(void *arg)
{
	media_format_t format = MEDIA_FMT_UNKNOWN;
	get_format_from_name(text_source, &format);

	if (uvocplayer->set_source(text_source)) {
        LOGE(TAG, "set source failed !\n");
        return NULL;
	}

	if (format == MEDIA_FMT_OPS || format == MEDIA_FMT_SPX)
        uvocplayer->set_pcminfo(source_sample_rate, 
                                source_channels, 16, 1280);

	if (uvocplayer->start()) {
		LOGE(TAG, "start failed !\n");
		uvocplayer->clr_source();
	}

	return NULL;
}

static int32_t play_local_mp3(void)
{
    int32_t random;

    random = rand() % 240;
    LOG("random: %d\n", random);
    memset(text_source, 0, sizeof(text_source));
    if (random < 100) {
        strcpy(text_source, "fs:/data/mp3/haas_intro.mp3");
    } else if (random > 100 && random < 150) {
        strcpy(text_source, "fs:/data/mp3/haasxiaozhushou.mp3");
    } else if (random > 150 && random < 200) {
        strcpy(text_source, "fs:/data/mp3/zhurenwozai.mp3");
    } else {
        strcpy(text_source, "fs:/data/mp3/eiwozai.mp3");
    }
    aos_task_new_ext(&play_task,
        "play music task", play_music,
        NULL, 8192, 0);

    return 0;
}

int32_t kws_callback(ai_result_t *result)
{
    int32_t kws_ret = (int32_t)*result;
    int32_t ret = -1;
    player_state_t player_state = -1;

    if (kws_ret) {
        beeper_start(0, 1, 25);
        OLED_Clear();
        OLED_Show_String(14, 24, "Hi, I am here!", 16, 1);
        OLED_Refresh_GRAM();
        beeper_stop(0);

        /*play local asr*/
        play_local_mp3();

        ret = uvocplayer->wait_complete();
        if (ret < 0)
            aos_msleep(1000);

        OLED_Clear();
        OLED_Show_String(28, 16, "HaaS HaaS!", 16, 1);
        OLED_Show_String(28, 34, "Wakeup me!", 16, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_Refresh_GRAM();
    }

    return 0;
}

int32_t kws_init(void)
{
    int32_t ret = 0;
    ai_config_t config;

    OLED_Clear();
    OLED_Show_String(28, 16, "HaaS HaaS!", 16, 1);
    OLED_Show_String(28, 34, "Wakeup me!", 16, 1);
    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
    OLED_Refresh_GRAM();

    if (!running) {
        /*Init sound driver*/
        audio_install_codec_driver();

        /*Init uvoice to play mp3*/
        uvoice_init();

        /*create uvoice player*/
        uvocplayer = uvoice_player_create();
        if (!uvocplayer) {
            LOGE(TAG, "create media player failed !\n");
            return -1;
        }
        /*set eq*/
        uvocplayer->eq_enable(0);

        /*set play volume*/
        uvocplayer->set_volume(10);
        running = true;
    }

    ret = aiagent_service_init("kws", AI_MODEL_KWS);
    if (ret < 0) {
        LOGE(TAG, "aiagent_service_init failed");
        return -1;
    }

    /*config mic channel*/
    config.channel = 0;
    aiagent_service_config(&config);

    return aiagent_service_model_infer(NULL, NULL, kws_callback);
}

int32_t kws_uninit(void)
{
    // running = false;
    uvocplayer->stop();
    uvocplayer->clr_source();
    aiagent_service_uninit();
    return 0;
}
