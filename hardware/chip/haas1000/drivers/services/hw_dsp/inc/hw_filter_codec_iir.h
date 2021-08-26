#ifndef __HW_FILTER_CODEC_IIR_H__
#define __HW_FILTER_CODEC_IIR_H__

#include <stdint.h>
#include "iir_process.h"
#include "hw_codec_iir_process.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    int32_t                 bypass;
    HW_CODEC_IIR_TYPE_T     iir_device;
#if 1
    HW_CODEC_IIR_CFG_F      iir_cfg;
#else
    IIR_CFG_T               iir_cfg;
#endif
} hw_filter_codec_iir_cfg;

struct hw_filter_codec_iir_state_;

typedef struct hw_filter_codec_iir_state_ hw_filter_codec_iir_state;

// Creat a instance from hw_filter_codec_iir module/class
// Common value include: sample rate, frame size and so on. 
hw_filter_codec_iir_state *hw_filter_codec_iir_create(int32_t sample_rate, int32_t channel_num, int32_t bits, hw_filter_codec_iir_cfg *cfg);

// Destory a hw_filter_codec_iir instance
int32_t hw_filter_codec_iir_destroy(hw_filter_codec_iir_state *st);

// Just use modify instance configure
int32_t hw_filter_codec_iir_set_config(hw_filter_codec_iir_state *st, hw_filter_codec_iir_cfg *cfg);

// Get/set some value or enable/disable some function
// int32_t hw_filter_codec_iir_ctl(hw_filter_codec_iir_state *st, int32_t ctl, void *ptr);

// Do not need process function, because hardware run separately

// Debug hw_filter_codec_iir instance
int32_t hw_filter_codec_iir_dump(hw_filter_codec_iir_state *st);

#ifdef __cplusplus
}
#endif

#endif
