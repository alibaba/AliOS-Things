
#include "aos/kernel.h"
#include "speech_memory.h"
#include "aqe_kws.h"
#include "mcu_audio.h"
#include "ulog/ulog.h"
#include "aiagent_engine.h"
#include "aiagent_service.h"
#include "engine/kws_engine.h"
#include "a7_cmd.h"

#define TAG "kws"

AqeKwsConfig kws_cfg;
AqeKwsState *kws_st = NULL;
uint32_t kws_cnt = 0;
static bool kws_running = false;
static aiagent_engine_t *eng = NULL;
static aos_task_t kws_task_handle;
static aos_sem_t kws_sem;
static int32_t kws_ret = 1;
static int8_t kws_channel = 0;
static bool kws_exited = false;
static bool kws_inited = false;

static void callback_main(void *p)
{
    kws_running = true;
    kws_exited = false;
    while (1) {
        aos_sem_wait(&kws_sem, AOS_WAIT_FOREVER);
        if (kws_running) {
            if (eng->callback)
                eng->callback((ai_result_t)&kws_ret);
            else
                LOG("callback is not set\n");
        }
        else
            break;
    }
    kws_exited = true;
}

static void my_kws_process(uint8_t *buf, uint32_t len)
{
    int32_t r = 0, w = 0;
    int16_t thres_tmp[1] = {60};
    A7_CMD_T *cmd = (A7_CMD_T *)buf;

    if (kws_exited)
        return;

    if ((cmd->p1 == 1) && (cmd->p2 == 1)) {
        LOG("wakeup\n");
        aos_sem_signal(&kws_sem);
    }
}

int32_t kws_engine_init(aiagent_engine_t *eng)
{
   if (!eng)
        return -1;

    /*enable dsp kws*/
    enable_a7_kws(1);

    if (!kws_inited) {
        aos_sem_new(&kws_sem, 0);
        set_a7_cmd_callback_handler(my_kws_process);
        kws_inited = true;
        LOGI(TAG, "aos_task_new_ext kws_task\n");
    }

    return 0;
}

static void kws_engine_delete(aiagent_engine_t *eng)
{
    if (!eng)
        return;

    free(eng);
}

static bool kws_engine_available(void)
{
    return true;
}

int32_t kws_engine_uninit(aiagent_engine_t *eng)
{
    int32_t ret;

    if (!eng)
        return -1;

    /*enable dsp kws*/
    enable_a7_kws(0);

    kws_running = false;
    aos_sem_signal(&kws_sem);
    while (!kws_exited) {
        aos_msleep(50);
    }
    aos_task_delete(&kws_task_handle);

    kws_st = NULL;
    LOGI(TAG, "aos_task_delete kws_task\n");
    return 0;
}

int32_t kws_engine_config(aiagent_engine_t *eng)
{
    kws_engine_config_t *config;
    config = (kws_engine_config_t *)eng->config;
    kws_channel = config->channel;
}

int32_t kws_engine_model_infer(aiagent_engine_t *eng)
{
    if (!eng)
        return -1;

    aos_task_new_ext(&kws_task_handle,
        "kws_callback", callback_main,
        NULL, 1024 * 120, AOS_DEFAULT_APP_PRI);

    return 0;
}

static aiagent_engine_t *kws_engine_create(int engineid)
{
    /* Malloc ai agent eng*/
    eng = (aiagent_engine_t *) malloc(sizeof(aiagent_engine_t));
    if (!eng) {
        LOGE(TAG, "malloc camera device fail\n");
        return NULL;
    }
    eng->is_dummy = false;

    /* Set the function pointers */
    eng->ai_engine_init = kws_engine_init;
    eng->ai_engine_uninit = kws_engine_uninit;
    eng->ai_engine_config = kws_engine_config;
    eng->ai_engine_model_infer = kws_engine_model_infer;
    eng->ai_engine_free = kws_engine_delete;

    return eng;
}

aiagent_context_t kws_engine = {
    KWS_NAME, "kws(keyword spotting) engine",
    kws_engine_available, kws_engine_create
};
