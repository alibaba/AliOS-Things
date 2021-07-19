
#include "aos/kernel.h"
#include "speech_memory.h"
#include "aqe_kws.h"
#include "mcu_audio.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "aiagent_engine.h"
#include "aiagent_service.h"
#include "engine/kws_engine.h"

#define TAG "kws"
#define APP_KWS_REAMPLERATE 8000
#define APP_KWS_PERIOD 16
#define APP_KWS_BITS 16
#define SPEECH_MEM_POOL_SIZE    (1024 * 100)
#define POSSIBLY_UNUSED                 __attribute__((unused))
static uint8_t POSSIBLY_UNUSED  speech_buf[SPEECH_MEM_POOL_SIZE];
#define APP_KWS_BUF_SIZE    (APP_KWS_REAMPLERATE / 1000 * APP_KWS_PERIOD * APP_KWS_BITS / 8)
static uint8_t POSSIBLY_UNUSED  kws_buf[APP_KWS_BUF_SIZE];

AqeKwsConfig kws_cfg;
AqeKwsState *kws_st = NULL;
static void *kws_handle;
uint32_t kws_cnt = 0;
static bool kws_running = false;
static aiagent_engine_t *eng = NULL;
static aos_task_t kws_task_handle;
static aos_sem_t kws_sem;
static int32_t kws_ret = 0;
static int8_t kws_channel = 0;
static bool kws_exited = false;

void speech_record_config_get(uint32_t *sample_rate, uint32_t *record_period_ms)
{
    if (sample_rate)
        *sample_rate = APP_KWS_REAMPLERATE;

    if (record_period_ms)
        *record_period_ms = APP_KWS_PERIOD;
}

static void callback_main(void *p)
{
    kws_running = true;
    kws_exited = false;
    while (1) {
        aos_sem_wait(&kws_sem, AOS_WAIT_FOREVER);
        if (kws_running)
            eng->callback((ai_result_t *)&kws_ret);
        else
            break;
    }
    kws_exited = true;
}

static void my_kws_process(uint8_t *buf, uint32_t len)
{
    int32_t r = 0, w = 0;
    int16_t thres_tmp[1] = {60};

    if (kws_exited)
        return;
    // retrieve first ch from 1 ch
    while (r < len) {
        kws_buf[w++] = buf[r + kws_channel * 2]; // 0: ch1, 1: ch2, 2: ch3
        kws_buf[w++] = buf[r + kws_channel * 2 + 1]; // 0: ch1, 1: ch2, 2: ch3
        r += 6; // 1 ch * 2
    }

    kws_ret = aqe_kws_process(kws_st, kws_buf, APP_KWS_BUF_SIZE / 2, thres_tmp);
    if (kws_ret) {
        kws_cnt++;
        LOGI(TAG, "[KWS]ret: %d, cnt: %d\n", kws_ret, kws_cnt);
        aos_sem_signal(&kws_sem);
    }
}

int32_t kws_engine_init(aiagent_engine_t *eng)
{
   if (!eng)
        return -1;

    if (!kws_st) {
        speech_heap_init(speech_buf, sizeof(speech_buf));
        kws_st = aqe_kws_create(APP_KWS_REAMPLERATE, APP_KWS_BUF_SIZE / 2, &kws_cfg);
        aos_sem_new(&kws_sem, 0);
        mcu_record_pre_handler_set(my_kws_process);
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

    kws_running = false;
    aos_sem_signal(&kws_sem);
    while (!kws_exited) {
        aos_msleep(50);
    }
    aos_task_delete(&kws_task_handle);
    // aqe_kws_destroy(kws_st);
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
