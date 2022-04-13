/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BT_STREAM_H__
#define __APP_BT_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_aud.h"
#include "stdint.h"
#define BT_AUDIO_BUFF_AAC_SIZE (1024*2*2*2)

#if defined(__3M_PACK__)
#define BT_AUDIO_BUFF_SBC_SIZE (128*7*2*2*2)
#else
#define BT_AUDIO_BUFF_SBC_SIZE (128*5*2*2*2)
#endif

#if defined(A2DP_LDAC_ON)
#define BT_AUDIO_BUFF_SIZE_LDAC (256*4*2*2*2)
#endif

#define BT_AUDIO_CACHE_2_UNCACHE(addr) \
    ((unsigned char *)((unsigned int)addr & ~(0x04000000)))

// #if BT_AUDIO_BUFF_SIZE < BT_AUDIO_SCO_BUFF_SIZE * 4
// #error BT_AUDIO_BUFF_SIZE must be at least BT_AUDIO_SCO_BUFF_SIZE * 4
// #endif

// low 11 bits are output streams index while high 5 bits are input streams index
// one output stream and one input stream can exit at the same time,
// while two streams with the same direction are mutex
#define APP_BT_STREAM_BORDER_BIT_OFFSET     12
// the index < the border is the output stream
// the index >= the border is the input stream
#define APP_BT_STREAM_BORDER_INDEX          (1 << APP_BT_STREAM_BORDER_BIT_OFFSET)

// output streams
#define APP_BT_STREAM_HFP_PCM               (1 << 0)
#define APP_BT_STREAM_HFP_CVSD              (1 << 1)
#define APP_BT_STREAM_HFP_VENDOR            (1 << 2)
#define APP_BT_STREAM_A2DP_SBC              (1 << 3)
#define APP_BT_STREAM_A2DP_AAC              (1 << 4)
#define APP_BT_STREAM_A2DP_VENDOR           (1 << 5)
#ifdef __FACTORY_MODE_SUPPORT__
#define APP_FACTORYMODE_AUDIO_LOOP          (1 << 6)
#endif
#define APP_PLAY_BACK_AUDIO                 (1 << 7)
#ifdef RB_CODEC
#define APP_BT_STREAM_RBCODEC               (1 << 8)
#endif
#ifdef AUDIO_LINEIN
#define APP_PLAY_LINEIN_AUDIO               (1 << 9)
#endif
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(__APP_A2DP_SOURCE__)||(APP_I2S_A2DP_SOURCE)
#define APP_A2DP_SOURCE_LINEIN_AUDIO        (1 << 10)
#define APP_A2DP_SOURCE_I2S_AUDIO           (1 << 11)
#endif


// input streams
#ifdef VOICE_DATAPATH
#define APP_BT_STREAM_VOICEPATH             (1 << APP_BT_STREAM_BORDER_BIT_OFFSET)
#endif

#ifdef __AI_VOICE__
#define APP_BT_STREAM_AI_VOICE             (1 << (APP_BT_STREAM_BORDER_BIT_OFFSET+1))
#endif

#define APP_BT_STREAM_INVALID               0

#define APP_BT_OUTPUT_STREAM_INDEX(player)  ((player) & ((1 << APP_BT_STREAM_BORDER_BIT_OFFSET) - 1))
#define APP_BT_INPUT_STREAM_INDEX(player)   ((player) & (~((1 << APP_BT_STREAM_BORDER_BIT_OFFSET) - 1)))

#include <stdint.h>

enum APP_BT_SETTING_T {
    APP_BT_SETTING_OPEN = 0,
    APP_BT_SETTING_CLOSE,
    APP_BT_SETTING_SETUP,
    APP_BT_SETTING_RESTART,
    APP_BT_SETTING_CLOSEALL,
    APP_BT_SETTING_CLOSEMEDIA,
    APP_BT_SETTING_NUM,
};

enum APP_BT_AUDIO_Q_POS {
    APP_BT_SETTING_Q_POS_HEAD = 0,
    APP_BT_SETTING_Q_POS_TAIL = 1,
};

typedef struct {
    uint16_t id;
    uint16_t status;

    uint16_t aud_type;
    uint16_t aud_id;

    uint8_t freq;
}APP_AUDIO_STATUS;

const char *player2str(uint16_t player_type);

int app_bt_stream_open(APP_AUDIO_STATUS* status);

int app_bt_stream_close(uint16_t player);

int app_bt_stream_setup(uint16_t player, uint8_t status);

int app_bt_stream_restart(APP_AUDIO_STATUS* status);

int app_bt_stream_closeall();

bool app_bt_stream_isrun(uint16_t player);

void app_bt_set_volume(uint16_t type,uint8_t level);

void app_bt_stream_volumeup(void);

void app_bt_stream_volumedown(void);

void app_bt_stream_volume_ptr_update(uint8_t *bdAddr);

struct btdevice_volume * app_bt_stream_volume_get_ptr(void);

int app_bt_stream_volumeset(int8_t vol);

uint8_t app_bt_stream_a2dpvolume_get(void);

uint8_t app_bt_stream_hfpvolume_get(void);

void app_bt_stream_a2dpvolume_reset(void);

void app_bt_stream_hfpvolume_reset(void);

void app_bt_stream_copy_track_one_to_two_24bits(int32_t *dst_buf, int32_t *src_buf, uint32_t src_len);

void app_bt_stream_copy_track_one_to_two_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t src_len);

void app_bt_stream_copy_track_two_to_one_16bits(int16_t *dst_buf, int16_t *src_buf, uint32_t dst_len);

enum AUD_SAMPRATE_T bt_get_sbc_sample_rate(void);

void bt_store_sbc_sample_rate(enum AUD_SAMPRATE_T sample_rate);

enum AUD_SAMPRATE_T bt_parse_store_sbc_sample_rate(uint8_t sbc_samp_rate);

bool app_bt_stream_is_mic_mix_during_music_enabled(void);
void app_bt_sream_set_mix_mic_flag(bool isEnable);
void bt_sbc_mix_mic_init(void);
void bt_sbc_mix_mic_deinit(void);
uint32_t bt_sbc_codec_capture_data(uint8_t *buf, uint32_t len);

void app_audio_buffer_check(void);

// =======================================================
// APP RESAMPLE
// =======================================================

#include "hal_aud.h"

typedef int (*APP_RESAMPLE_BUF_ALLOC_CALLBACK)(uint8_t **buff, uint32_t size);

typedef int (*APP_RESAMPLE_ITER_CALLBACK)(uint8_t *buf, uint32_t len);

struct APP_RESAMPLE_T {
    enum AUD_STREAM_T stream;
    void *id;
    APP_RESAMPLE_ITER_CALLBACK cb;
    uint8_t *iter_buf;
    uint32_t iter_len;
    uint32_t offset;
    float ratio_step;
};

struct APP_RESAMPLE_T *app_playback_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
                                                  APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len);
struct APP_RESAMPLE_T *app_playback_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
                                                      APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
                                                      float ratio_step);
struct APP_RESAMPLE_T *app_playback_resample_any_open_with_pre_allocated_buffer(enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
        float ratio_step, uint8_t* ptrBuf, uint32_t bufSize);                                                      
int app_playback_resample_close(struct APP_RESAMPLE_T *resamp);
int app_playback_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len);

struct APP_RESAMPLE_T *app_capture_resample_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_CHANNEL_NUM_T chans,
                                                 APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len);
struct APP_RESAMPLE_T *app_capture_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
                                                     APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
                                                     float ratio_step);
int app_capture_resample_close(struct APP_RESAMPLE_T *resamp);
int app_capture_resample_run(struct APP_RESAMPLE_T *resamp, uint8_t *buf, uint32_t len);

void app_resample_reset(struct APP_RESAMPLE_T *resamp);
void app_resample_tune(struct APP_RESAMPLE_T *resamp, float ratio);
APP_RESAMPLE_BUF_ALLOC_CALLBACK app_resample_set_buf_alloc_callback(APP_RESAMPLE_BUF_ALLOC_CALLBACK cb);
uint16_t app_bt_stream_ibrt_trigger_seq_diff_calc(int32_t dma_samples, int32_t frame_samples, int32_t sub_seq, int32_t interval);
void app_bt_stream_volumeset_handler(int8_t vol);

#ifdef TX_RX_PCM_MASK
void store_encode_frame2buff();
#endif

#ifdef BONE_SENSOR_TDM
void bt_sco_get_tdm_buffer(uint8_t **buf, uint32_t *len);
#endif
int app_bt_stream_local_volume_get(void);

bool bt_is_playback_triggered(void);

struct APP_RESAMPLE_T *app_force48k_resample_any_open(enum AUD_CHANNEL_NUM_T chans,
        APP_RESAMPLE_ITER_CALLBACK cb, uint32_t iter_len,
        float ratio_step);

#ifdef __cplusplus
}
#endif

#endif
