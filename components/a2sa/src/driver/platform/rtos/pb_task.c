/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef HAAS_AUDIO_DEMO
#include <posix/pthread.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <errno.h>
#include "ulog/ulog.h"
#include "audio_rtos.h"

#define LOG_TAG    "[pb_task]"
#define PB_HIGH_STACKSIZE            8192
#define PB_DEFAULT_PRIORITY          33
#define PB_TIMER_INTERVAL_MS         20    // minimum interval 20ms
#define PB_TIMER_MAX_IDLE_TIME_MS    2000  // max idle time 2s
#define PB_DATA_BUFFER_ENTRY_NUM_MAX 10

typedef struct {
    struct dlist_s list;
    char *data;
    unsigned int size;
    unsigned int wOffset;
    void *dev;
} pb_data_buf_t;

static pthread_cond_t  pb_cond;
static pthread_mutex_t  pb_mutex;
static pthread_t pb_thread;
static bool bCreatePbThreadFlag = false;
static bool bPbStartFlag = false;
static aos_hdl_t flagMutex;
AOS_DLIST_HEAD(pb_data_list);

static void pb_thread_loop(void *arg)
{
    pb_data_buf_t *data_buf = NULL;
    native_pcm_device_t *pcm_dev = NULL;
    dlist_t *temp = NULL;
    unsigned int frame_size, period_bytes, left_bytes;
    unsigned int sleepCntUs;
    long long now_ts = 0;
    static int totalIdleTime = 0;
    bool copyStartFlag = false;
    LOGD(LOG_TAG, "%s:%d, entry!! bPbStartFlag %d", __func__, __LINE__, bPbStartFlag);
    while(1) {
        now_ts = aos_calendar_localtime_get();
        aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
        copyStartFlag = bPbStartFlag;
        aos_mutex_unlock(&flagMutex);
        if(true == copyStartFlag) {
            sleepCntUs = PB_TIMER_INTERVAL_MS * 1000;
            if(dlist_empty(&pb_data_list)) {
                totalIdleTime ++;
                if(totalIdleTime > PB_TIMER_MAX_IDLE_TIME_MS / PB_TIMER_INTERVAL_MS) {
                    sleepCntUs = PB_TIMER_INTERVAL_MS * 100 * 1000;
                }
                LOGE(LOG_TAG, "%s:%d, data list empty, underflow!!", __func__, __LINE__);
            } else {
                data_buf = dlist_first_entry(&pb_data_list, pb_data_buf_t, list);
                if(data_buf && data_buf->data && data_buf->dev) {
                    pcm_dev = (native_pcm_device_t *)data_buf->dev;
                    frame_size = pcm_dev->params.channels * pcm_dev->params.sample_bits / 8;
                    period_bytes = frame_size * PB_TIMER_INTERVAL_MS * pcm_dev->params.rate / 1000;
                    left_bytes = data_buf->size - data_buf->wOffset;
                    if(left_bytes > period_bytes) {
                        pcm_dev->ops->write(pcm_dev->hdl, data_buf->data + data_buf->wOffset, period_bytes);
                        data_buf->wOffset += period_bytes;
                        //LOGE(LOG_TAG, "%s:%d, fetch %d bytes from buffer", __func__, __LINE__, period_bytes);
                    } else {
                        pcm_dev->ops->write(pcm_dev->hdl, data_buf->data + data_buf->wOffset, left_bytes);
                        data_buf->wOffset += left_bytes;
                        sleepCntUs = left_bytes * PB_TIMER_INTERVAL_MS * 1000 / period_bytes;
                        //LOGE(LOG_TAG, "%s:%d, fetch %d bytes from buffer", __func__, __LINE__, left_bytes);

                        /* free data_buf */
                        dlist_del(&data_buf->list);
                        free(data_buf->data);
                        free(data_buf);
                    }
                }
            }
            now_ts = (aos_calendar_localtime_get() - now_ts) * 1000;
            if(sleepCntUs > now_ts) {
                usleep(sleepCntUs - now_ts);
            }
        } else {
            LOGD(LOG_TAG, "%s:%d, wait for pb_cond", __func__, __LINE__);
            dlist_for_each_entry_safe(&pb_data_list, temp, data_buf, pb_data_buf_t, list) {
                if(data_buf) {
                    dlist_del(&data_buf->list);
                    free(data_buf->data);
                    free(data_buf);
                }
            }
            pthread_cond_wait(&pb_cond, &pb_mutex);
        }
    }

    return 0;
}

static int create_pb_task(void)
{
    if(bCreatePbThreadFlag == true) {
        LOGD(LOG_TAG, "%s:%d, pbthread is running, skip ...", __func__, __LINE__);
        return -1;
    }
    LOGD(LOG_TAG, "%s:%d, -->>", __func__, __LINE__);
    pthread_attr_t attr;
    struct sched_param sched;

    aos_mutex_new(&flagMutex);
    pthread_cond_init(&pb_cond, NULL);
    pthread_mutex_init(&pb_mutex, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PB_HIGH_STACKSIZE);
    sched.sched_priority = PB_DEFAULT_PRIORITY;
    pthread_attr_setschedparam(&attr, &sched);
    pthread_create(&pb_thread, &attr, pb_thread_loop, NULL);
    pthread_setname_np(pb_thread, "pbthread");
    pthread_attr_destroy(&attr);
    bCreatePbThreadFlag = true;
    return 0;
}

void playback_task_start()
{
    LOGE(LOG_TAG, "%s:%d", __func__, __LINE__);
    aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
    bPbStartFlag = true;
    aos_mutex_unlock(&flagMutex);
    create_pb_task();
    pthread_cond_signal(&pb_cond);
}

void playback_task_stop()
{
    LOGE(LOG_TAG, "%s:%d", __func__, __LINE__);
    aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
    bPbStartFlag = false;
    aos_mutex_unlock(&flagMutex);
}

int playback_task_drain()
{
    while(!dlist_empty(&pb_data_list)) {
        aos_msleep(PB_TIMER_INTERVAL_MS);
    }
    return 0;
}

int playback_push_data(void *dev, const char *buf, unsigned int len, int blockMode)
{
    if(1 == blockMode) {
        while(dlist_entry_number(&pb_data_list) >= PB_DATA_BUFFER_ENTRY_NUM_MAX) {
            usleep(PB_TIMER_INTERVAL_MS * 1000);
        }
    }
    if(dlist_entry_number(&pb_data_list) >= PB_DATA_BUFFER_ENTRY_NUM_MAX) {
        return 0;
    }
    pb_data_buf_t *databuf = malloc(sizeof(pb_data_buf_t));
    
    if(!databuf) {
        LOGE(LOG_TAG, "%s:%d, malloc pb_data_buf_t failed", __func__, __LINE__);
        return -1;
    }
    databuf->data = malloc(len);
    if(!databuf->data) {
        LOGE(LOG_TAG, "%s:%d, malloc %d bytes failed", __func__, __LINE__, len);
        return -1;
    }
    memset(databuf->data, 0, len);
    databuf->size = len;
    memcpy(databuf->data, buf, len);
    databuf->wOffset = 0;
    databuf->dev = dev;
    dlist_init(&databuf->list);
    dlist_add_tail(&databuf->list, &pb_data_list);
    //LOGE(LOG_TAG, "%s:%d: push %d bytes to buffer", __func__, __LINE__, len);
    return len;
}