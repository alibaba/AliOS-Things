/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
#ifndef __APP_A2DP_SOURCE_H__
#define __APP_A2DP_SOURCE_H__

#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "analog.h"
#include "bt_drv.h"
#include "app_audio.h"
#include "bt_drv_interface.h"
#include "app_bt_stream.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"
#include "a2dp_api.h"
#include "avrcp_api.h"

#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "apps.h"
#include "resources.h"
#include "app_bt_media_manager.h"
#include "tgt_hardware.h"
#include "app_utils.h"
#include "app_bt_func.h"
#include "a2dp_api.h"

#define APP_SOURCE_DEBUG
#ifdef APP_SOURCE_DEBUG
#define SOURCE_DBLOG TRACE
#else
#define SOURCE_DBLOG(...)
#endif


extern struct BT_DEVICE_T  app_bt_device;


//#define A2DP_LINEIN_SIZE    (48*2*1024)
//#define A2DP_TRANS_SIZE     2048

#define A2DP_LINEIN_SIZE    (10*2*1024)
#define A2DP_TRANS_SIZE     1024*2

typedef struct  {
    osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#ifdef CMSIS_OS_RTX
    uint32_t _semaphore_data[2];
#endif

} a2dp_source_lock_t;


typedef struct{
    CQueue  pcm_queue;
    osThreadId sbc_send_id;
    a2dp_source_lock_t  data_lock;
    a2dp_source_lock_t  sbc_send_lock;
    enum AUD_SAMPRATE_T sample_rate;
    uint8_t sbcen_samplerate;


}A2DP_SOURCE_STRUCT;


typedef struct sbcpack{
    btif_a2dp_sbc_packet_t sbcPacket;
    char buffer[A2DP_TRANS_SIZE];
    int free;
}sbcpack_t;

typedef struct sbcbank{
    sbcpack_t sbcpacks[1];
    int free;
}sbcbank_t;

extern A2DP_SOURCE_STRUCT  a2dp_source;
extern void a2dp_source_notify_send(void);
extern uint32_t a2dp_source_linein_more_pcm_data(uint8_t * pcm_buf, uint32_t len);
#endif
#endif
