/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SubBandAec_H__
#define __SubBandAec_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int bypass;
    int filter_size;
} SubBandAecConfig;

struct SubBandAecState_;

typedef struct SubBandAecState_ SubBandAecState;

SubBandAecState *SubBandAec_init(int sample_rate, int frame_size, const SubBandAecConfig *cfg);

void SubBandAec_destroy(SubBandAecState *st);

int32_t SubBandAec_process(SubBandAecState *st, short *datain, short *echoref, short *dataout, short Len);

float SubBandAec_get_required_mips(SubBandAecState *st);

#ifdef __cplusplus
}
#endif

#endif

