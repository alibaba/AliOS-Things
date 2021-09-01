


#ifndef __KWS_EXT_FFT_H__
#define __KWS_EXT_FFT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

void *ext_f32_fft_init(int32_t size, int32_t mode, void *ext_fft_malloc(int));
void ext_f32_fft_destroy(void *table, void ext_fft_free(void *));
void ext_f32_fft(void *table, float *in, float *out);
void ext_f32_ifft(void *table, float *in, float *out);

#ifdef __cplusplus
}
#endif

#endif

