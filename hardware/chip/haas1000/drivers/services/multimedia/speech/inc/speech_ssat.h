/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_SSAT_H__
#define __SPEECH_SSAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && defined(__arm__)
#include "cmsis.h"
static inline short speech_ssat_int16(int in)
{
    short out;

    out = __SSAT(in,16);

    return out;
}

static inline int speech_ssat_int24(int in)
{
    short out;

    out = __SSAT(in, 24);

    return out;
}
#else
static inline short speech_ssat_int16(int in)
{
    short out;

    if (in>32767)
    {
        in = 32767;
    }
    else if (in<-32768)
    {
        in = -32768;
    }
    out = (int)in;
    return out;
}

static inline int speech_ssat_int24(int in)
{
    short out;

    if (in > 0x7fffff) {
        in = 0x7fffff;
    } else if (in < 0x800000) {
        in = 0x800000;
    }
    out = (int)in;
    return out;
}
#endif

#ifdef __cplusplus
}
#endif

#endif