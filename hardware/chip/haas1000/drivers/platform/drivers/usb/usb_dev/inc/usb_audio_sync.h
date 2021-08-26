/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __USB_AUDIO_SYNC_H__
#define __USB_AUDIO_SYNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

typedef uint32_t (*USB_AUDIO_SYNC_TIME_TO_MS)(uint32_t time);

enum UAUD_SYNC_RET_T {
    UAUD_SYNC_IDLE,
    UAUD_SYNC_START,
    UAUD_SYNC_ERR,
};

struct USB_AUDIO_STREAM_INFO_T {
    // Difference error threshold
    uint16_t err_thresh;
    // Difference synchronization threshold
    uint16_t sync_thresh;
    // Sample rate
    uint32_t samp_rate;
    // Total sample count of the buffer
    uint32_t samp_cnt;
    // The number of value to calculate the average
    uint8_t diff_avg_cnt;
    // The stream info ID
    uint8_t id;
    // Whether to enable diff_target
    uint8_t diff_target_enabled;
    // The Max adjust ratio moving to diff_target
    float max_target_ratio;
    // The abosulte difference value between current diff and diff_target to apply max_target_ratio
    uint32_t max_target_thresh;
    // The target of the difference
    int diff_target;
    // Function to convert timestamp to ms. If NULL, default timer will be used.
    USB_AUDIO_SYNC_TIME_TO_MS time_to_ms;
    // Current timestamp. If time_to_ms is NULL, this member will be ignored.
    uint32_t time;
    // Internal control block
    uint32_t ctrl_block[8];
};

void usb_audio_sync_reset(struct USB_AUDIO_STREAM_INFO_T *info);

int usb_audio_sync_normalize_diff(int diff, uint32_t size);

enum UAUD_SYNC_RET_T usb_audio_sync(uint32_t pos_a, uint32_t pos_b, struct USB_AUDIO_STREAM_INFO_T *info, float *p_ratio);

#ifdef __cplusplus
}
#endif

#endif
