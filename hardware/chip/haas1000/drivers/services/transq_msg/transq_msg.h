/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __TRANSQ_MSG_H__
#define __TRANSQ_MSG_H__

#if defined(CHIP_HAAS1000)
#include "hal_transq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TRANSQ_MSG_HANDLE_CB_T)(void *param);

typedef enum {
    TRANSQ_MSG_TYPE_TRACE,
    TRANSQ_MSG_TYPE_AF_OPEN,
    TRANSQ_MSG_TYPE_AF_CLOSE,
    TRANSQ_MSG_TYPE_AF_CAPTURE_START,
    TRANSQ_MSG_TYPE_AF_CAPTURE_STOP,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_START,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_STOP,
    TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_IRQ,
    TRANSQ_MSG_TYPE_RECORD_DATA,
    TRANSQ_MSG_TYPE_USERDATA,
    TRANSQ_MSG_TYPE_VOIP,
    TRANSQ_MSG_TYPE_KEY,
    TRANSQ_MSG_TYPE_CMD,
    TRANSQ_MSG_TYPE_AUDIO_DUMP,
    TRANSQ_MSG_TYPE_HEARTBEAT,
    TRANSQ_MSG_TYPE_AUDIO_DUMP_ONE_CHANNEL,
    TRANSQ_MSG_TYPE_NUM,
}TRANSQ_MSG_TYPE_T;

struct TRANSQ_MSG_TRACE {
    unsigned int addr;
    unsigned int len;
};

struct TRANSQ_MSG_AF_CONFIG_T {
    unsigned int bits;
    unsigned int sample_rate;
    unsigned int channel_num;
    unsigned int channel_map;
    unsigned int device;
    unsigned int io_path;
    bool chan_sep_buf;
    bool i2s_master_clk_wait;
    unsigned char i2s_sample_cycles;
    unsigned char *data_ptr;
    unsigned int data_size;

    //should define type
    unsigned char vol;
};

struct TRANSQ_MSG_AF_BUF {
    unsigned char *buf;
    unsigned int len;
};


struct TRANSQ_MSG{
    struct TRANSQ_MSG_TRACE trace;
    struct TRANSQ_MSG_AF_CONFIG_T stream_cfg;
    struct TRANSQ_MSG_AF_BUF stream_buf;
};

typedef struct {
    TRANSQ_MSG_TYPE_T type;
    enum HAL_TRANSQ_PRI_T pri;
    unsigned int id;
    struct TRANSQ_MSG msg;
    void *user_data;
    unsigned int user_data_len;
    unsigned char sync;
} TRANSQ_MSG_T;

#ifndef RTOS
#define transq_msg_tx_wait_done(p) \
do{ \
    transq_tx_done = 0; \
    if(transq_msg_tx(p)) { \
        while (!transq_tx_done) { \
            hal_sys_timer_delay(MS_TO_TICKS(1)); \
        } \
        hal_sys_timer_delay(MS_TO_TICKS(1)); \
    } \
}while(0)
#else
#define transq_msg_tx_wait_done(p) \
do{ \
    if(transq_msg_tx(p)) { \
        if (transq_tx_sem != NULL) { \
            osSemaphoreWait(transq_tx_sem, osWaitForever); \
        } \
    } \
}while(0)
#endif

int transq_msg_init();
int transq_msg_reinit();
int transq_msg_flush();
void transq_msg_onoff(int onoff);
int transq_msg_tx(TRANSQ_MSG_T *msg);
void transq_msg_register(TRANSQ_MSG_TYPE_T type, TRANSQ_MSG_HANDLE_CB_T func, bool tx);

#ifdef __cplusplus
}
#endif

#endif
#endif
