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

#define LOG_TAG                       "[cap_task]"
#define CAP_HIGH_STACKSIZE            8192
#define CAP_DEFAULT_PRIORITY          33
#define CAP_TIMER_DEFAULT_INTERVAL_MS 20    // default capture thread interval 20ms
#define CAP_DATA_BUFFER_ENTRY_NUM_MAX 50    // default capture data buffer size 1s

typedef struct {
    struct dlist_s list;
    char *data;
    unsigned int size;
    unsigned int wOffset;
    void *dev;
} cap_data_buf_t;

static native_pcm_device_t *pcm_dev = NULL;
static pthread_cond_t  cap_cond;
static pthread_mutex_t  cap_mutex;
static pthread_t cap_thread;
static bool bCreateCapThreadFlag = false;
static bool bCapStartFlag = false;
static unsigned int cap_interval_ms = CAP_TIMER_DEFAULT_INTERVAL_MS;
static aos_hdl_t flagMutex;
AOS_DLIST_HEAD(cap_data_list);

static void capture_task_drop()
{
    dlist_t *temp = NULL;
    cap_data_buf_t *data_buf = NULL;
    dlist_for_each_entry_safe(&cap_data_list, temp, data_buf, cap_data_buf_t, list) {
        if(data_buf) {
            dlist_del(&data_buf->list);
            free(data_buf->data);
            free(data_buf);
        }
    }
}

static void cap_thread_loop(void *arg)
{
    cap_data_buf_t *databuf = NULL;
    unsigned int frame_size, period_bytes;
    bool copyStartFlag = false;
    while(1) {
        aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
        copyStartFlag = bCapStartFlag;
        aos_mutex_unlock(&flagMutex);
        if(true == copyStartFlag) {
            if(dlist_entry_number(&cap_data_list) >= CAP_DATA_BUFFER_ENTRY_NUM_MAX) {
                LOGE(LOG_TAG, "%s:%d, capure data list full, report buf overflow event to APP!!", __func__, __LINE__);
            } else if(pcm_dev && pcm_dev->ops && pcm_dev->ops->read) {
                frame_size = pcm_dev->params.channels * pcm_dev->params.sample_bits / 8;
                period_bytes = frame_size * cap_interval_ms * pcm_dev->params.rate / 1000;
                databuf = malloc(sizeof(cap_data_buf_t));
                if(databuf) {
                    databuf->data = malloc(period_bytes);
                    if(databuf->data) {
                        memset(databuf->data, 0, period_bytes);
                        databuf->size = pcm_dev->ops->read(pcm_dev->hdl, databuf->data, period_bytes);
                        databuf->wOffset = 0;
                        databuf->dev = pcm_dev;
                        dlist_init(&databuf->list);
                        dlist_add_tail(&databuf->list, &cap_data_list);
                    }
                }
            } else {
                LOGE(LOG_TAG, "%s:%d, invalid pcm_dev params", __func__, __LINE__);
            }
            usleep(cap_interval_ms * 1000);
        } else {
            LOGE(LOG_TAG, "%s:%d, wait for cap_cond", __func__, __LINE__);
            capture_task_drop();
            pthread_cond_wait(&cap_cond, &cap_mutex);
        }
    }

    return 0;
}

static int create_cap_task(void)
{
    if(bCreateCapThreadFlag == true) {
        LOGD(LOG_TAG, "%s:%d, capthread is running, skip ...", __func__, __LINE__);
        return -1;
    }
    LOGD(LOG_TAG, "%s:%d, -->>", __func__, __LINE__);
    pthread_attr_t attr;
    struct sched_param sched;

    aos_mutex_new(&flagMutex);
    pthread_cond_init(&cap_cond, NULL);
    pthread_mutex_init(&cap_mutex, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, CAP_HIGH_STACKSIZE);
    sched.sched_priority = CAP_DEFAULT_PRIORITY;
    pthread_attr_setschedparam(&attr, &sched);
    pthread_create(&cap_thread, &attr, cap_thread_loop, NULL);
    pthread_setname_np(cap_thread, "capthread");
    pthread_attr_destroy(&attr);
    bCreateCapThreadFlag = true;
    return 0;
}

void capture_task_start(void *dev)
{
    LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
    }
    pcm_dev = dev;
    aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
    bCapStartFlag = true;
    aos_mutex_unlock(&flagMutex);
    create_cap_task();
    pthread_cond_signal(&cap_cond);
}

void capture_task_stop()
{
    LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    aos_mutex_lock(&flagMutex, AOS_WAIT_FOREVER);
    bCapStartFlag = false;
    aos_mutex_unlock(&flagMutex);
}

int capture_read_data(void *dev, char *buf, unsigned int len, int blockMode)
{
    dlist_t *temp = NULL;
    cap_data_buf_t *data_buf = NULL;
    unsigned int left_bytes = len;
    unsigned int read_bytes = 0;

__read_routin__:
    dlist_for_each_entry_safe(&cap_data_list, temp, data_buf, cap_data_buf_t, list) {
        if(data_buf && (data_buf->dev == dev)) {
            if((data_buf->size - data_buf->wOffset) >= left_bytes) {
                memcpy(buf + read_bytes, data_buf->data + data_buf->wOffset, left_bytes);
                read_bytes += left_bytes;
                data_buf->wOffset += left_bytes;
                left_bytes = 0;
                return read_bytes;
            } else {
                memcpy(buf + read_bytes, data_buf->data + data_buf->wOffset, data_buf->size - data_buf->wOffset);
                read_bytes += data_buf->size - data_buf->wOffset;
                data_buf->wOffset = data_buf->size;
                left_bytes -= data_buf->size - data_buf->wOffset;
            }
            if(data_buf->wOffset >= data_buf->size) {
                /* free data_buf */
                dlist_del(&data_buf->list);
                free(data_buf->data);
                free(data_buf);
            }
        }
    }
    if(1 == blockMode && read_bytes < len) {
        usleep(cap_interval_ms * 1000);
        goto __read_routin__;
    }
    return read_bytes;
}
