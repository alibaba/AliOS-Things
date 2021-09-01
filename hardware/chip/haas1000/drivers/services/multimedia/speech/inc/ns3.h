/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef NS3_H
#define NS3_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    NS3_SUPPRESSION_LOW = 0,
    NS3_SUPPRESSION_MID = 1,
    NS3_SUPPRESSION_HIGH = 2,
    NS3_SUPPRESSION_VERY_HIGH = 3
} NS3_SUPPRESSION_MODE;

typedef struct
{
    int32_t     bypass;
    NS3_SUPPRESSION_MODE mode;
} Ns3Config;

struct Ns3State_;

typedef struct Ns3State_ Ns3State;

#define CONSTRUCT_FUNC_NAME_A(p, c, m)          p ## _ ## c ## _ ## m
#define CONSTRUCT_FUNC_NAME(p, c, m)            CONSTRUCT_FUNC_NAME_A(p, c, m)

#ifndef NS3_IMPL
#define NS3_IMPL fixed
#endif

#define ns3_create CONSTRUCT_FUNC_NAME(ns3, NS3_IMPL, init)
#define ns3_destroy CONSTRUCT_FUNC_NAME(ns3, NS3_IMPL, destroy)
#define ns3_set_config CONSTRUCT_FUNC_NAME(ns3, NS3_IMPL, set_config)
#define ns3_process CONSTRUCT_FUNC_NAME(ns3, NS3_IMPL, process)
#define ns3_get_required_mips CONSTRUCT_FUNC_NAME(ns3, NS3_IMPL, get_required_mips)

Ns3State *ns3_create(int32_t sample_rate, int32_t frame_size, const Ns3Config *cfg);

int32_t ns3_destroy(Ns3State *st);

int32_t ns3_set_config(Ns3State *st, const Ns3Config *cfg);

int32_t ns3_process(Ns3State *st, int16_t *pcm_buf, int32_t pcm_len);

float ns3_get_required_mips(Ns3State *st);

#ifdef __cplusplus
}
#endif

#endif