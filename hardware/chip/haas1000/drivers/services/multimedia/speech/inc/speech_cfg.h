/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_CFG_H__
#define __SPEECH_CFG_H__

// Speech process module
#include "speech_dc_filter.h"
#include "speech_iir_calibration.h"
#include "speech_fir_calibration.h"
#include "speech_aec.h"
#include "speech_aec2.h"
#include "echo_canceller.h"
#include "dual_mic_denoise.h"
#include "speech_2mic_ns2_denoise.h"
#include "sensormic_denoise.h"
#include "leftright_denoise.h"
#include "speech_3mic_ns.h"
#include "triple_mic_denoise3.h"
#include "far_field_speech_enhancement.h"
#include "speech_ns.h"
#include "lc_mmse_ns.h"
#include "lc_mmse_ns_float.h"
#include "ns3.h"
#include "wnr.h"
#include "VoiceActivityDetection.h"
#include "speech_noise_gate.h"
#include "compexp.h"
#include "agc.h"
#include "speech_eq.h"
#include "speech_gain.h"
#include "speech_ff_2mic_ns2.h"
#include "SubBandBasedAEC.h"
// Tool module
#include "speech_utils.h"
#include "speech_ssat.h"
#include "speech_memory.h"
#include "iir_resample.h"
#include "frame_resize.h"
#include "ae_math.h"

// Other module
#include "wind_detection_2mic.h"

#endif