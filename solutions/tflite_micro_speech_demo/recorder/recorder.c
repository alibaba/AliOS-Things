/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ulog/ulog.h>
#include "uvoice_init.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_recorder.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "uvoice_os.h"

#define TAG "recorder"
static uvoice_recorder_t *mrecorder = NULL;

int32_t recorder_init(void)
{
    mrecorder = uvoice_recorder_create();
    if (!mrecorder) {
        LOGE(TAG, "create uvoice recorder failed !\n");
        return -1;
    }

    return 0;
}

int32_t recorder_uninit(void)
{
    return mrecorder->stop();
}

int32_t recorder_start(media_format_t fmt, int32_t rate, int32_t channels, int32_t bits, int32_t samples, char *sink)
{
    int32_t len = 0, ret = 0;

    ret = mrecorder->set_sink(fmt, rate, channels, bits, samples, 0, sink);
    if (ret < 0) {
        LOGE(TAG, "uvoice recorder set_sink failed !\n");
        return -1;
    }
    ret = mrecorder->start();
    if (ret < 0) {
        LOGE(TAG, "uvoice recorder start failed !\n");
        return -1;
    }
    return 0;
}

int32_t recorder_stop(void)
{
    return mrecorder->clr_sink();
}

int32_t recorder_get_stream(uint8_t *buf, int32_t read_size)
{
    return mrecorder->get_stream(buf, read_size);
}
