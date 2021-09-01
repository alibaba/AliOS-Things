/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef FRAME_RESIZE_H
#define FRAME_RESIZE_H

#include "stdint.h"

struct FrameResizeState_;

typedef struct FrameResizeState_ FrameResizeState;

typedef int32_t(*CAPTURE_HANDLER_T)(void *, void *, int32_t *);

typedef int32_t(*PLAYBACK_HANDLER_T)(void *, int32_t *);

#ifdef __cplusplus
extern "C" {
#endif

FrameResizeState *frame_resize_create(int codec_frame_size,
                                    int codec_capture_channel,
                                    int vqe_frame_size,
                                    int capture_sample_size,
                                    int playback_sample_size,
                                    int aec_enable,
                                    const CAPTURE_HANDLER_T capture_handler,
                                    const PLAYBACK_HANDLER_T playback_handler);

void frame_resize_destroy(FrameResizeState *st);

void frame_resize_process_capture(FrameResizeState *st, void *pcm_buf, void *ref_buf, int32_t *pcm_len);

void frame_resize_process_playback(FrameResizeState *st, void *pcm_buf, int32_t *pcm_len);

#ifdef __cplusplus
}
#endif

#endif