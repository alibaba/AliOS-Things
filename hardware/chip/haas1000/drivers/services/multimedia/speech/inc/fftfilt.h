#ifndef FFTFILT_H
#define FFTFILT_H

#include <stdint.h>

typedef struct
{
    float *filter;
    int filter_length;
    int nfft;
} FftFiltConfig;

struct FftFiltState_;

typedef struct FftFiltState_ FftFiltState;

#ifdef __cplusplus
extern "C" {
#endif

FftFiltState *fftfilt_init(int sample_rate, int frame_size, const FftFiltConfig *config);

void fftfilt_destroy(FftFiltState *st);

void fftfilt_process_float(FftFiltState *st, float *buf, int frame_size);

void fftfilt_process(FftFiltState *st, int16_t *buf, int frame_size);

void fftfilt_process2_float(FftFiltState *st, float *buf, int frame_size, int stride);

void fftfilt_process2(FftFiltState *st, int16_t *buf, int frame_size, int stride);

#ifdef __cplusplus
}
#endif

#endif