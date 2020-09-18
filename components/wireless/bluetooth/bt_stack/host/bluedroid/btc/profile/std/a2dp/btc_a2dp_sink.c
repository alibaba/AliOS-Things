// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/******************************************************************************
 **
 **  Name:          btc_a2dp_sink.c
 **
 ******************************************************************************/
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include <string.h>
#include <stdint.h>
#include "common/bt_defs.h"
#include "osi/allocator.h"
#include "osi/mutex.h"
#include "osi/thread.h"
#include "osi/fixed_queue.h"
#include "osi/alarm.h"
#include "stack/a2d_api.h"
#include "stack/a2d_sbc.h"
#include "bta/bta_av_api.h"
#include "bta/bta_av_ci.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc/btc_manage.h"
#include "btc_av.h"
#include "btc/btc_util.h"
#include "btc_audio_a2dp_sink.h"
#include "yoc_a2dp_api.h"
#include "oi_codec_sbc.h"
#include "oi_status.h"
#include "osi/future.h"
#include <assert.h>

#if (BTC_AV_SINK_INCLUDED == TRUE)

extern osi_thread_t *btc_thread;

/*****************************************************************************
 **  Constants
 *****************************************************************************/

/* BTC media cmd event definition : BTC_MEDIA_TASK_CMD */
enum {
    BTC_MEDIA_TASK_SINK_INIT,
    BTC_MEDIA_TASK_SINK_CLEAN_UP,
    BTC_MEDIA_FLUSH_AA_RX,
    BTC_MEDIA_AUDIO_SINK_CFG_UPDATE,
    BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK,
};

enum {
    BTC_A2DP_SINK_STATE_OFF = 0,
    BTC_A2DP_SINK_STATE_ON = 1,
    BTC_A2DP_SINK_STATE_SHUTTING_DOWN = 2
};

/*
 * CONGESTION COMPENSATION CTRL ::
 *
 * Thus setting controls how many buffers we will hold in media task
 * during temp link congestion. Together with the stack buffer queues
 * it controls much temporary a2dp link congestion we can
 * compensate for. It however also depends on the default run level of sinks
 * jitterbuffers. Depending on type of sink this would vary.
 * Ideally the (SRC) max tx buffer capacity should equal the sinks
 * jitterbuffer runlevel including any intermediate buffers on the way
 * towards the sinks codec.
 */

/* fixme -- define this in pcm time instead of buffer count */

/* The typical runlevel of the tx queue size is ~1 buffer
   but due to link flow control or thread preemption in lower
   layers we might need to temporarily buffer up data */

/* 18 frames is equivalent to 6.89*18*2.9 ~= 360 ms @ 44.1 khz, 20 ms mediatick */
#define MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ       (50)
#define START_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ     (30)


#define BTC_SINK_MEDIA_TIME_TICK_MS             20

typedef struct {
    uint32_t sig;
    void *param;
} a2dp_sink_task_evt_t;

typedef struct {
    UINT16 num_frames_to_be_processed;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} tBT_SBC_HDR;

typedef struct {
    BOOLEAN rx_flush; /* discards any incoming data when true */
    UINT8   channel_count;
    UINT8   bits_per_sample;
    fixed_queue_t *RxStreamQ;
    UINT32  sample_rate;

    const tA2DP_DECODER_INTERFACE* decoder_interface;
    void *player_handle;
    osi_alarm_t * decode_alarm;
} tBTC_A2DP_SINK_CB;

typedef struct {
    tBTC_A2DP_SINK_CB   btc_aa_snk_cb;
    future_t            *btc_a2dp_sink_future;
    osi_thread_t        *btc_aa_snk_task_hdl;
} a2dp_sink_local_param_t;

static void btc_a2dp_sink_thread_init(UNUSED_ATTR void *context);
static void btc_a2dp_sink_thread_cleanup(UNUSED_ATTR void *context);
static void btc_a2dp_sink_flush_q(fixed_queue_t *p_q);
static void btc_a2dp_sink_rx_flush(void);
static int btc_a2dp_sink_get_track_frequency(UINT8 frequency);
static int btc_a2dp_sink_get_track_channel_count(UINT8 channeltype);
/* Handle incoming media packets A2DP SINK streaming*/
static bool btc_a2dp_sink_handle_inc_media(tBT_SBC_HDR *p_msg);
static void btc_a2dp_sink_handle_decoder_reset(tBTC_MEDIA_SINK_CFG_UPDATE *p_msg);
static void btc_a2dp_sink_handle_clear_track(void);
static BOOLEAN btc_a2dp_sink_clear_track(void);
static void btc_a2dp_sink_ctrl_handler(void *arg);
static void btc_a2dp_sink_data_ready(void *context);

static int btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;
static yoc_a2d_sink_data_cb_t bt_aa_snk_data_cb = NULL;
#if A2D_DYNAMIC_MEMORY == FALSE
static a2dp_sink_local_param_t a2dp_sink_local_param;
#else
static a2dp_sink_local_param_t *a2dp_sink_local_param_ptr;
#define a2dp_sink_local_param (*a2dp_sink_local_param_ptr)
#endif ///A2D_DYNAMIC_MEMORY == FALSE

void btc_a2dp_sink_reg_data_cb(yoc_a2d_sink_data_cb_t callback)
{
    // todo: critical section protection
    bt_aa_snk_data_cb = callback;
}

static inline void btc_a2d_data_cb_to_app(const uint8_t *data, uint32_t len)
{
    // todo: critical section protection
    if (bt_aa_snk_data_cb) {
        bt_aa_snk_data_cb(data, len);
    }
}


/*****************************************************************************
 **  Misc helper functions
 *****************************************************************************/
static inline void btc_a2d_cb_to_app(yoc_a2d_cb_event_t event, yoc_a2d_cb_param_t *param)
{
    yoc_a2d_cb_t btc_aa_cb = (yoc_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_aa_cb) {
        btc_aa_cb(event, param);
    }
}

/*****************************************************************************
 **  BTC ADAPTATION
 *****************************************************************************/

static bool btc_a2dp_sink_ctrl_post(uint32_t sig, void *param)
{
    a2dp_sink_task_evt_t *evt = (a2dp_sink_task_evt_t *)osi_malloc(sizeof(a2dp_sink_task_evt_t));

    if (evt == NULL) {
        return false;
    }

    evt->sig = sig;
    evt->param = param;

    return osi_thread_post(a2dp_sink_local_param.btc_aa_snk_task_hdl,  btc_a2dp_sink_ctrl_handler, evt, 1, OSI_THREAD_MAX_TIMEOUT);
}

static void btc_a2dp_sink_ctrl_handler(void *arg)
{
    a2dp_sink_task_evt_t *e = (a2dp_sink_task_evt_t *)arg;

    if (e == NULL) {
        return;
    }

    switch (e->sig) {
    case BTC_MEDIA_TASK_SINK_INIT:
        btc_a2dp_sink_thread_init(NULL);
        break;
    case BTC_MEDIA_TASK_SINK_CLEAN_UP:
        btc_a2dp_sink_thread_cleanup(NULL);
        break;
    case BTC_MEDIA_AUDIO_SINK_CFG_UPDATE:
        btc_a2dp_sink_handle_decoder_reset(e->param);
        break;
    case BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK:
        btc_a2dp_sink_handle_clear_track();
        break;
    case BTC_MEDIA_FLUSH_AA_RX:
        btc_a2dp_sink_rx_flush();
        break;
    default:
        APPL_TRACE_WARNING("media task unhandled evt: 0x%x\n", e->sig);
    }

    if (e->param != NULL) {
        osi_free(e->param);
    }

    osi_free(e);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_startup
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
bool btc_a2dp_sink_startup(void)
{
    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_OFF) {
        APPL_TRACE_ERROR("warning : media task already running");
        return false;
    }

#if A2D_DYNAMIC_MEMORY == TRUE
    if ((a2dp_sink_local_param_ptr = (a2dp_sink_local_param_t *)osi_malloc(sizeof(a2dp_sink_local_param_t))) == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return false;
    }
    memset((void *)a2dp_sink_local_param_ptr, 0, sizeof(a2dp_sink_local_param_t));
#endif

    APPL_TRACE_EVENT("## A2DP SINK START MEDIA THREAD ##");

    a2dp_sink_local_param.btc_aa_snk_task_hdl = btc_thread;

    if (btc_a2dp_sink_ctrl_post(BTC_MEDIA_TASK_SINK_INIT, NULL) == false) {
        goto error_exit;
    }

    APPL_TRACE_EVENT("## A2DP SINK MEDIA THREAD STARTED ##\n");

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread\n", __func__);
    a2dp_sink_local_param.btc_aa_snk_task_hdl = NULL;

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(a2dp_sink_local_param_ptr);
    a2dp_sink_local_param_ptr = NULL;
#endif

    return false;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_shutdown
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
void btc_a2dp_sink_shutdown(void)
{
    APPL_TRACE_EVENT("## A2DP SINK STOP MEDIA THREAD ##\n");

    // Exit thread
    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_SHUTTING_DOWN;
    a2dp_sink_local_param.btc_a2dp_sink_future = future_new();
    assert(a2dp_sink_local_param.btc_a2dp_sink_future);
    btc_a2dp_sink_ctrl_post(BTC_MEDIA_TASK_SINK_CLEAN_UP, NULL);
    future_await(a2dp_sink_local_param.btc_a2dp_sink_future);
    a2dp_sink_local_param.btc_a2dp_sink_future = NULL;

    a2dp_sink_local_param.btc_aa_snk_task_hdl = NULL;

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(a2dp_sink_local_param_ptr);
    a2dp_sink_local_param_ptr = NULL;
#endif
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_on_idle
**
*******************************************************************************/
void btc_a2dp_sink_on_idle(void)
{
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();
    btc_a2dp_sink_clear_track();

    APPL_TRACE_DEBUG("Stopped BT track");
    btc_a2dp_sink_audio_handle_stop_decoding();
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_on_stopped
**
*******************************************************************************/
void btc_a2dp_sink_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();
    btc_a2dp_control_set_datachnl_stat(FALSE);

    APPL_TRACE_DEBUG("Stopped BT track");
    btc_a2dp_sink_audio_handle_stop_decoding();
}

/*****************************************************************************
**
** Function        btc_a2dp_on_suspended
**
*******************************************************************************/
void btc_a2dp_sink_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();

    APPL_TRACE_DEBUG("Stopped BT track");
    btc_a2dp_sink_audio_handle_stop_decoding();
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_alarm_data_post
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static void btc_a2dp_sink_alarm_data_post(void)
{
    osi_thread_post(a2dp_sink_local_param.btc_aa_snk_task_hdl, btc_a2dp_sink_data_ready, NULL, 2, OSI_THREAD_MAX_TIMEOUT);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_clear_track
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static BOOLEAN btc_a2dp_sink_clear_track(void)
{
    return btc_a2dp_sink_ctrl_post(BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK, NULL);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_set_rx_flush
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
void btc_a2dp_sink_set_rx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP RX %d ##\n", enable);
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = enable;
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_reset_decoder
**
** Description
**
** Returns
**
*******************************************************************************/
void btc_a2dp_sink_reset_decoder(UINT8 *p_av)
{
    APPL_TRACE_EVENT("btc reset decoder");
    APPL_TRACE_DEBUG("btc reset decoder p_codec_info[%x:%x:%x:%x:%x:%x]\n",
                     p_av[1], p_av[2], p_av[3],
                     p_av[4], p_av[5], p_av[6]);

    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_SINK_CFG_UPDATE)))) {
        APPL_TRACE_ERROR("btc reset decoder No Buffer ");
        return;
    }

    memcpy(p_buf->codec_info, p_av, AVDT_CODEC_SIZE);
    btc_a2dp_sink_ctrl_post(BTC_MEDIA_AUDIO_SINK_CFG_UPDATE, p_buf);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_data_ready
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_data_ready(UNUSED_ATTR void *context)
{
    tBT_SBC_HDR *p_msg;

    if (fixed_queue_is_empty(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ)) {
        APPL_TRACE_DEBUG("QUEUE  EMPTY");
    } else {
        if (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush == TRUE) {
            btc_a2dp_sink_flush_q(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ);
            return;
        }

        while ((p_msg = (tBT_SBC_HDR *)fixed_queue_try_peek_first(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ)) != NULL ) {
            if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_ON) {
                return;
            }
            if (btc_a2dp_sink_handle_inc_media(p_msg) == false) {
                return;
            }
            p_msg = (tBT_SBC_HDR *)fixed_queue_dequeue(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ, 0);
            if (p_msg == NULL) {
                APPL_TRACE_ERROR("Insufficient data in queue ");
                break;
            }
            osi_free(p_msg);
        }
        APPL_TRACE_DEBUG(" Process Frames - ");
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_handle_decoder_reset
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_handle_decoder_reset(tBTC_MEDIA_SINK_CFG_UPDATE *p_msg)
{
    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf = p_msg;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    OI_STATUS       status;
    UINT32          freq_multiple = 48 * 20; /* frequency multiple for 20ms of data , initialize with 48K*/
    UINT32          num_blocks = 16;
    UINT32          num_subbands = 8;

    APPL_TRACE_DEBUG("%s p_codec_info[%x:%x:%x:%x:%x:%x]\n", __func__,
                     p_buf->codec_info[1], p_buf->codec_info[2], p_buf->codec_info[3],
                     p_buf->codec_info[4], p_buf->codec_info[5], p_buf->codec_info[6]);

    int sample_rate = A2DP_GetTrackSampleRate(p_buf->codec_info);
    if (sample_rate == -1) {
        APPL_TRACE_ERROR("%s: cannot get the track frequency", __func__);
        return;
    }
    int bits_per_sample = A2DP_GetTrackBitsPerSample(p_buf->codec_info);
    if (bits_per_sample == -1) {
        APPL_TRACE_ERROR("%s: cannot get the bits per sample", __func__);
        return;
    }
    int channel_count = A2DP_GetTrackChannelCount(p_buf->codec_info);
    if (channel_count == -1) {
        APPL_TRACE_ERROR("%s: cannot get the channel count", __func__);
        return;
    }
    int channel_type = A2DP_GetSinkTrackChannelType(p_buf->codec_info);
    if (channel_type == -1) {
        APPL_TRACE_ERROR("%s: cannot get the Sink channel type", __func__);
        return;
    }
    a2dp_sink_local_param.btc_aa_snk_cb.sample_rate = sample_rate;
    a2dp_sink_local_param.btc_aa_snk_cb.bits_per_sample = bits_per_sample;
    a2dp_sink_local_param.btc_aa_snk_cb.channel_count = channel_count;

    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = FALSE;
    
    APPL_TRACE_EVENT("Reset to sink role");
    a2dp_sink_local_param.btc_aa_snk_cb.decoder_interface = A2DP_GetDecoderInterface(p_buf->codec_info);
    if (a2dp_sink_local_param.btc_aa_snk_cb.decoder_interface == NULL) {
        APPL_TRACE_ERROR("%s: cannot stream audio: no source decoder interface",
              __func__);
        return;
    }

    if (!a2dp_sink_local_param.btc_aa_snk_cb.decoder_interface->decoder_init()) {
        APPL_TRACE_ERROR("%s: failed to initialize decoder", __func__);
        return;
    }

    btc_a2dp_control_set_datachnl_stat(TRUE);

    a2dp_sink_local_param.btc_aa_snk_cb.player_handle = btc_audio_create_player(sample_rate, bits_per_sample, channel_count);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_handle_inc_media
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static bool btc_a2dp_sink_handle_inc_media(tBT_SBC_HDR *p_msg)
{
    uint8_t *outputBuffer;
    int32_t outputSize;
    
    /* Check if the below check is correct, we are checking for peer to be sink when we are sink */
    if (btc_av_get_peer_sep() == AVDT_TSEP_SNK || (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush)) {
        APPL_TRACE_DEBUG(" State Changed happened in this tick ");
        return true;
    }

    // ignore data if no one is listening
    if (!btc_a2dp_control_get_datachnl_stat()) {
        return true;
    }

    if(a2dp_sink_local_param.btc_aa_snk_cb.decoder_interface == NULL) {
        APPL_TRACE_ERROR("none decoder interface");
        return true;
    }

    /* should check first play flag and wait buffer */
    if (btc_audio_player_get_underRun()) {
        if (fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ) < START_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ) {
            return false;
        }
    }

    /* should check output buffer */
    if (btc_audio_player_getRestBufferSize() <= 4096) {
        return false;
    }
    
    if (!a2dp_sink_local_param.btc_aa_snk_cb.decoder_interface->decode_packet(p_msg, &outputBuffer, &outputSize)) {
        APPL_TRACE_ERROR("%s: decoding failed", __func__);
        return true;
    }
#if 1
    btc_audio_player_writeBuffer(outputBuffer, outputSize);
#endif
    return true;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_rx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_sink_rx_flush_req(void)
{
    if (fixed_queue_is_empty(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ) == TRUE) { /*  Que is already empty */
        return TRUE;
    }

    return btc_a2dp_sink_ctrl_post(BTC_MEDIA_FLUSH_AA_RX, NULL);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_rx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_rx_flush(void)
{
    /* Flush all enqueued SBC  buffers (encoded) */
    APPL_TRACE_DEBUG("btc_a2dp_sink_rx_flush");

    btc_a2dp_sink_flush_q(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_audio_handle_start_decoding
 **
 ** Description      This function is called when start decoding
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
static void btc_a2dp_sink_audio_handle_start_decoding(void)
{
    APPL_TRACE_DEBUG("%s", __func__);
    
    if (a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm != NULL)
        return;  // Already started decoding
  
    btc_audio_start_player(a2dp_sink_local_param.btc_aa_snk_cb.player_handle);
  
    a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm = osi_alarm_new("a2dp_sink_decode", btc_a2dp_sink_alarm_data_post, NULL, 0);
    if (a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm == NULL) {
        APPL_TRACE_ERROR("%s: unable to allocate decode alarm", __func__);
        return;
    }
    osi_alarm_set_periodic(a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm, BTC_SINK_MEDIA_TIME_TICK_MS);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_audio_handle_stop_decoding
 **
 ** Description      This function is called when stop decoding
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
void btc_a2dp_sink_audio_handle_stop_decoding(void)
{
    osi_alarm_t* old_alarm;

    APPL_TRACE_DEBUG("%s", __func__);

    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = true;
    btc_a2dp_sink_rx_flush_req();
    old_alarm = a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm;
    a2dp_sink_local_param.btc_aa_snk_cb.decode_alarm = NULL;

    // Drop the lock here, btif_decode_alarm_cb may in the process of being called
    // while we alarm free leading to deadlock.
    //
    // alarm_free waits for btif_decode_alarm_cb which is waiting for g_mutex.
    osi_alarm_free(old_alarm);

    btc_audio_stop_player(a2dp_sink_local_param.btc_aa_snk_cb.player_handle);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btc_a2dp_sink_enque_buf(BT_HDR *p_pkt)
{
    tBT_SBC_HDR *p_msg;

    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_ON){
        return 0;
    }

    if (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush == TRUE) { /* Flush enabled, do not enque*/
        return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ);
    }

    if (fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ) >= MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ) {
        APPL_TRACE_WARNING("Pkt dropped\n");
        return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ);
    }

    APPL_TRACE_DEBUG("btc_a2dp_sink_enque_buf + ");

    /* allocate and Queue this buffer */
    if ((p_msg = (tBT_SBC_HDR *) osi_malloc(sizeof(tBT_SBC_HDR) +
                                            p_pkt->offset + p_pkt->len)) != NULL) {
        memcpy(p_msg, p_pkt, (sizeof(BT_HDR) + p_pkt->offset + p_pkt->len));
        p_msg->num_frames_to_be_processed = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
        APPL_TRACE_VERBOSE("btc_a2dp_sink_enque_buf %d + \n", p_msg->num_frames_to_be_processed);
        fixed_queue_enqueue(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ, p_msg, FIXED_QUEUE_MAX_TIMEOUT);
        btc_a2dp_sink_audio_handle_start_decoding();
    } else {
        /* let caller deal with a failed allocation */
        APPL_TRACE_WARNING("btc_a2dp_sink_enque_buf No Buffer left - ");
    }
    return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_handle_clear_track
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_handle_clear_track(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    btc_audio_stop_player(a2dp_sink_local_param.btc_aa_snk_cb.player_handle);
    btc_audio_destroy_player(a2dp_sink_local_param.btc_aa_snk_cb.player_handle);
    a2dp_sink_local_param.btc_aa_snk_cb.player_handle = NULL;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_flush_q(fixed_queue_t *p_q)
{
    while (! fixed_queue_is_empty(p_q)) {
        osi_free(fixed_queue_dequeue(p_q, 0));
    }

    btc_audio_player_flushBuffer();
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_thread_init
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_thread_init(UNUSED_ATTR void *context)
{
    APPL_TRACE_EVENT("%s\n", __func__);
    memset(&a2dp_sink_local_param.btc_aa_snk_cb, 0, sizeof(a2dp_sink_local_param.btc_aa_snk_cb));

    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_ON;

    a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ = fixed_queue_new(QUEUE_SIZE_MAX);

    btc_a2dp_control_init();
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_thread_cleanup
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_thread_cleanup(UNUSED_ATTR void *context)
{
    btc_a2dp_control_set_datachnl_stat(FALSE);
    /* Clear task flag */
    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;

    btc_a2dp_control_cleanup();

    fixed_queue_free(a2dp_sink_local_param.btc_aa_snk_cb.RxStreamQ, osi_free_func);

    future_ready(a2dp_sink_local_param.btc_a2dp_sink_future, NULL);
}

#endif /* BTC_AV_SINK_INCLUDED */
