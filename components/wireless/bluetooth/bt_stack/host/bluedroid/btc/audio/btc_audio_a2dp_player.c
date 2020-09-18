/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "common/bt_target.h"
#include "common/bt_trace.h"

#if BTC_AV_SINK_INCLUDED

#include "osi/mutex.h"
#include "btc_audio_a2dp_player.h"
#include "bt_hal_os.h"
#include "tg_audio.h"

#define PLAY_BUFFER_SIZE        16384
#define PLAYER_LOCK_TIMEOUT     100

#define BTC_AUDIO_PLAY_STACK_SIZE       4096
#define BTC_AUDIO_PLAY_STACK_PRIO       (BT_TASK_DEFAULT_PRIORITIES - 1)

typedef struct _A2DP_PlayerBuffer{
    uint8_t buffer[PLAY_BUFFER_SIZE];
    uint32_t bufferWritePtr;
    uint32_t bufferReadPtr;
    osi_mutex_t bufferMutex;
    hal_task_t play_task;
    uint8_t underRunOccur;
    uint8_t state;
    uint8_t flag;
    uint8_t task_created;
    tg_pcm_handle_t dev_handle;
}A2DP_PlayerBuffer;

A2DP_PlayerBuffer g_btc_audio_a2dp_playerBuffer;

int32_t btc_audio_player_readBuffer(uint8_t *buffer, int32_t size);
int btc_audio_player_writeToDev(uint8_t *buffer, int32_t size);
int32_t btc_audio_player_getBufferSize(void);

/*******************************************************************************
 **
 ** Function         btc_audio_player_set_underRun
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void btc_audio_player_set_underRun(uint8_t flag)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;

    player->underRunOccur = flag;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_get_underRun
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
uint8_t btc_audio_player_get_underRun(void)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;

    return player->underRunOccur;
}


/*******************************************************************************
 **
 ** Function         _btc_audio_player_state_callback
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void _btc_audio_player_state_callback(enum tg_pcm_state state, void *user, void *info)
{
    switch (state) {
    case TG_PCM_STATE_UNDERRUN:
        BTC_TRACE_DEBUG("%s: TG_PCM_STATE_UNDERRUN\n", __func__);
        btc_audio_player_set_underRun(1);
        break;
    case TG_PCM_STATE_UNDERRUN_EXIT:
        BTC_TRACE_DEBUG("%s: TG_PCM_STATE_UNDERRUN_EXIT\n", __func__);

        break;
    case TG_PCM_STATE_OVERRUN:
        BTC_TRACE_DEBUG("%s: TG_PCM_STATE_OVERRUN\n", __func__);
        break;
    case TG_PCM_STATE_OVERRUN_EXIT:
        BTC_TRACE_DEBUG("%s: TG_PCM_STATE_OVERRUN_EXIT\n", __func__);
        break;
    default:
        break;
    }
}

/*******************************************************************************
 **
 ** Function         btc_audio_create_player
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_audio_player_thread(void *arg)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    int32_t bufferSize, readSize;
    uint8_t *buffer;

    BTC_TRACE_DEBUG("%s enter", __func__);
    while (1) {
        if (player->dev_handle == NULL) {
            hal_os_delay(100);
            continue;
        }

        bufferSize = btc_audio_player_getBufferSize();
        if (bufferSize == 0) {
            hal_os_delay(10);
            continue;
        }

        if (bufferSize > 1024)
            bufferSize = 1024;

        buffer = malloc(bufferSize);
        if (buffer == NULL)
            return;

        readSize = btc_audio_player_readBuffer(buffer, bufferSize);
        BTC_TRACE_DEBUG("%s buffSize = %d, readSize = %d", __func__, bufferSize, readSize);
        btc_audio_player_writeToDev(buffer, readSize);
        free(buffer);
    }
}

/*******************************************************************************
 **
 ** Function         btc_audio_create_player
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void * btc_audio_create_player(int32_t sample_rate, int32_t bits_per_sample, int32_t channel_type)
{
    int8_t format;
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    BTC_TRACE_DEBUG("%s, sample_rate = %d, bits_per_sample = %d, channel_type = %d", __func__, sample_rate, bits_per_sample, channel_type);

    if (player->flag == 1)
        osi_mutex_free(&(player->bufferMutex));

    if (player->dev_handle)
        return;

    osi_mutex_new(&(player->bufferMutex));
    player->bufferWritePtr = 0;
    player->bufferReadPtr = 0;
    player->state = 0;
    player->flag = 1;
#if 1
    if (player->task_created == 0) {
        if (hal_os_thread_new(&(player->play_task), "PCM Player", btc_audio_player_thread, NULL, BTC_AUDIO_PLAY_STACK_SIZE, BTC_AUDIO_PLAY_STACK_PRIO)) {
            BTC_TRACE_ERROR("%s: hal_os_thread_new failed", __func__);
            btc_audio_destroy_player(player->dev_handle);
            return NULL;
        }
        player->task_created = 1;
    }
#endif

    /* audio dev create */
    switch (bits_per_sample) {
    case 8:
        format = TG_PCM_FORMAT_S8;
        break;
    case 16:
        format = TG_PCM_FORMAT_S16_LE;
        break;
    case 24:
        format = TG_PCM_FORMAT_S24_LE;
        break;
    }

    //player->dev_handle = tg_pcm_open(TG_PCM_OUT, sample_rate, channel_type, format);
    if (player->dev_handle == NULL) {
        BTC_TRACE_ERROR("%s: tg_pcm_open failed", __func__);
        btc_audio_destroy_player(player->dev_handle);
        return NULL;
    }

    tg_pcm_set_state_listener(player->dev_handle, _btc_audio_player_state_callback, NULL);

    return (void *)player;
}

/*******************************************************************************
 **s
 ** Function         btc_audio_destroy_player
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void btc_audio_destroy_player(void *handle)
{
    A2DP_PlayerBuffer *player = (A2DP_PlayerBuffer *)handle;

    BTC_TRACE_DEBUG("%s", __func__);
    if (player != &g_btc_audio_a2dp_playerBuffer) {
        BTC_TRACE_ERROR("%s wrong handle", __func__);
        return;
    }

    if ((player == 0) || (player->flag == 0)) {
        BTC_TRACE_ERROR("%s without create", __func__);
        return;
    }

    /* audio dev destroy */
    if (player->dev_handle) {
        //tg_pcm_close(player->dev_handle);
        player->dev_handle = NULL;
    }
    if (player->flag) {
        osi_mutex_free(&player->bufferMutex);
        player->flag = 0;
    }
}

/*******************************************************************************
 **
 ** Function         btc_audio_start_player
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void btc_audio_start_player(void *handle)
{
    A2DP_PlayerBuffer *player = (A2DP_PlayerBuffer *)handle;

    BTC_TRACE_DEBUG("%s", __func__);
    if (player != &g_btc_audio_a2dp_playerBuffer) {
        BTC_TRACE_ERROR("%s wrong handle", __func__);
        return;
    }

    if ((player == 0) || (player->flag == 0)) {
        BTC_TRACE_ERROR("%s without create", __func__);
        return;
    }
    osi_mutex_lock(&(player->bufferMutex), PLAYER_LOCK_TIMEOUT);
    player->bufferWritePtr = 0;
    player->bufferReadPtr = 0;
    player->state = 1;
    btc_audio_player_set_underRun(1);
    osi_mutex_unlock(&(player->bufferMutex));

    if (player->dev_handle) {
        //tg_pcm_start(player->dev_handle);
//        tg_volume_set(player->dev_handle, 80);
    }
}

/*******************************************************************************
 **
 ** Function         btc_audio_stop_player
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void btc_audio_stop_player(void *handle)
{
    A2DP_PlayerBuffer *player = (A2DP_PlayerBuffer *)handle;

    BTC_TRACE_DEBUG("%s", __func__);
    if (player != &g_btc_audio_a2dp_playerBuffer) {
        BTC_TRACE_ERROR("%s wrong handle", __func__);
        return;
    }

    if ((player == 0) || (player->flag == 0)) {
        BTC_TRACE_ERROR("%s without create", __func__);
        return;
    }
    osi_mutex_lock(&(player->bufferMutex), PLAYER_LOCK_TIMEOUT);
    player->bufferWritePtr = 0;
    player->bufferReadPtr = 0;
    player->state = 0;
    osi_mutex_unlock(&(player->bufferMutex));

    //if (player->dev_handle)
        //tg_pcm_stop(player->dev_handle);
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_getBufferSize
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int32_t btc_audio_player_getBufferSize(void)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    int32_t size;

    osi_mutex_lock(&(player->bufferMutex), PLAYER_LOCK_TIMEOUT);
    size = player->bufferWritePtr - player->bufferReadPtr;
    if (size < 0)
        size += PLAY_BUFFER_SIZE;

    osi_mutex_unlock(&(player->bufferMutex));
    return size;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_getRestBufferSize
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int32_t btc_audio_player_getRestBufferSize(void)
{
    return PLAY_BUFFER_SIZE - btc_audio_player_getBufferSize()-4;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_writeBuffer
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int32_t btc_audio_player_writeBuffer(uint8_t *buffer, int32_t size)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    int32_t writeSize, part1Size;

    if (buffer == NULL)
        return 0;

    writeSize = size;

    /* should not full */
    part1Size = btc_audio_player_getRestBufferSize();
    if (part1Size < writeSize)
        writeSize = part1Size;

    part1Size = writeSize;
    if (writeSize+player->bufferWritePtr > PLAY_BUFFER_SIZE) {
        part1Size = PLAY_BUFFER_SIZE-player->bufferWritePtr;
    }
    memcpy(player->buffer+player->bufferWritePtr, buffer, part1Size);
    if (writeSize > part1Size) {
        memcpy(player->buffer, buffer+part1Size, writeSize-part1Size);
    }

    player->bufferWritePtr += writeSize;
    if (player->bufferWritePtr >= PLAY_BUFFER_SIZE)
        player->bufferWritePtr -= PLAY_BUFFER_SIZE;

    return writeSize;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_readBuffer
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int32_t btc_audio_player_readBuffer(uint8_t *buffer, int32_t size)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    int32_t readSize, part1Size;

    if (buffer == NULL)
        return 0;

    readSize = size;
    part1Size = btc_audio_player_getBufferSize();
    if (part1Size < readSize)
        readSize = part1Size;

    part1Size = readSize;
    if (readSize+player->bufferReadPtr > PLAY_BUFFER_SIZE) {
        part1Size = PLAY_BUFFER_SIZE-player->bufferReadPtr;
    }
    memcpy(buffer, player->buffer+player->bufferReadPtr, part1Size);
    if (readSize > part1Size) {
        memcpy(buffer+part1Size, player->buffer, readSize-part1Size);
    }

    player->bufferReadPtr += readSize;
    if (player->bufferReadPtr >= PLAY_BUFFER_SIZE)
        player->bufferReadPtr -= PLAY_BUFFER_SIZE;

    return readSize;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_readBuffer
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int btc_audio_player_writeToDev(uint8_t *buffer, int32_t size)
{
    int32_t retsize;

    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    BTC_TRACE_DEBUG("%s, size = %d", __func__, size);

    if (btc_audio_player_get_underRun()) {
        btc_audio_player_set_underRun(0);
    }

    if (player->dev_handle == NULL) {
        BTC_TRACE_ERROR("%s:dev handle NULL", __func__);
        return 0;
    }

    //retsize = tg_pcm_write(player->dev_handle, buffer, size);
    BTC_TRACE_DEBUG("%s write, retsize = %d", __func__, retsize);

    return retsize;
}

/*******************************************************************************
 **
 ** Function         btc_audio_player_flushBuffer
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void btc_audio_player_flushBuffer(void)
{
    A2DP_PlayerBuffer *player = &g_btc_audio_a2dp_playerBuffer;
    int32_t size;

    osi_mutex_lock(&(player->bufferMutex), PLAYER_LOCK_TIMEOUT);
    player->bufferWritePtr = 0;
    player->bufferReadPtr = 0;
    osi_mutex_unlock(&(player->bufferMutex));
}
#endif
