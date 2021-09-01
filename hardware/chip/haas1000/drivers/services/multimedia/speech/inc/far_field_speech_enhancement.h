/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __FAR_FIELD_SPEECH_ENHANCEMENT_H__
#define __FAR_FIELD_SPEECH_ENHANCEMENT_H__

#ifdef __cplusplus
extern "C" {
#endif

void far_field_speech_enhancement_init(void);
void far_field_speech_enhancement_start(void);
void far_field_speech_enhancement_process(short *in);
void far_field_speech_enhancement_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
