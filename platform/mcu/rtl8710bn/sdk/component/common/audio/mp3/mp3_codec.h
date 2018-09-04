#ifndef __MINIMP3_DEC_H_INCLUDED__
#define __MINIMP3_DEC_H_INCLUDED__

#define MP3_MAX_SAMPLES_PER_FRAME (1152*2)

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct _mp3_info {
    int sample_rate;
    int channels;
    int audio_bytes;  // generated amount of audio per frame
} mp3_info_t;

typedef void* mp3_decoder_t;

extern mp3_decoder_t mp3_create(void);
extern int mp3_decode(mp3_decoder_t *dec, void *buf, int bytes, signed short *out, mp3_info_t *info);
extern void mp3_done(mp3_decoder_t *dec);
#define mp3_free(dec) do { mp3_done(dec); dec = NULL; } while(0)

#endif//__MINIMP3_DEC_H_INCLUDED__