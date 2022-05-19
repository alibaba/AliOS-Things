#ifndef DUER_DEVICE_VAD_H
#define DUER_DEVICE_VAD_H

#ifdef __cplusplus
extern "C" {
#endif

// Copyright 2017 Baidu Inc. All Rights Reserved.
// Author: liuzhiyu (liuzhiyu04@baidu.com)
//
// device vad

// default , do not modify
#define SAMPLE_RATE (16000)
#define VAD_10MS_SIZE (SAMPLE_RATE/100*2)
#define VAD_20MS_SIZE (SAMPLE_RATE/100*4)
#define VAD_FRAME_MS (80) // 80ms
#define VAD_INPUT_MS (20) // 20ms
#define VAD_CACHE_SIZE (VAD_FRAME_MS/VAD_INPUT_MS)
#define VAD_VOICE_NUM_IN_FRAME (2) // if a frame which is  80ms len,  have >= 20ms voice, detect speaking
// can change
#define VAD_IGNORE_HOTWORD_TIME (2 * VAD_FRAME_MS) // 2*80 ms
#define VAD_SPEAKING_SILENCE_VALID_TIME  (6 * VAD_FRAME_MS) // 6*80 ms
#define VAD_VOICE_VALID_TIME (5 * VAD_FRAME_MS) // 5*80 ms

// human stop speak
int vad_stop_speak(void);

void vad_stop_speak_done(void);

// human start speak
int vad_start_speak(void);

void vad_start_speak_done(void);

// run vad state machine
int device_vad(char *buff, size_t length);

// when wakeup use this function
void vad_set_wakeup(void);

// when cloud vad work, use this function
void vad_set_playing(void);

// ms recommendation is a multiple of VAD_FRAME_MS
void vad_set_silence_valid_time(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif
