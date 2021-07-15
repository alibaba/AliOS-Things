#ifndef __AQE_KWS_H__
#define __AQE_KWS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
} AqeKwsConfig;

struct AqeKwsState_;
typedef struct AqeKwsState_ AqeKwsState;

AqeKwsState *aqe_kws_create(uint32_t sample_rate, uint32_t frame_size, AqeKwsConfig *cfg);
int32_t aqe_kws_destroy(AqeKwsState *st);
int32_t aqe_kws_process(AqeKwsState *st, int16_t *pcm_in, uint32_t pcm_len, int16_t *thres_buf);

float aqe_kws_get_required_mips(AqeKwsState *st);

#ifdef __cplusplus
}
#endif

#endif