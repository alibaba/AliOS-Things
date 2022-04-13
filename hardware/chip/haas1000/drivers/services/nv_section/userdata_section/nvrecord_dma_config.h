/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef NEW_NV_RECORD_ENALBED
#ifdef NVREC_BAIDU_DATA_SECTION

#ifndef __NVRECORD_DMA_CONFIG_H__
#define __NVRECORD_DMA_CONFIG_H__

#include "nvrecord_extension.h"

#define BAIDU_DATA_DEF_RAND "abcdefghi"

#ifdef __cplusplus
extern "C" {
#endif

int nvrec_baidu_data_init(void);
int nvrec_get_fm_freq(void);
int nvrec_set_fm_freq(int fmfreq);
int nvrec_get_rand(char *rand);
int nvrec_set_rand(char *rand);
void nvrecord_rebuild_dma_configuration(NV_DMA_CONFIGURATION_T* pDmaConfig);
int nvrec_dev_get_sn(char *sn);

#ifdef __cplusplus
}
#endif
#endif
#endif  // #ifdef NVREC_BAIDU_DATA_SECTION
#endif  // #ifdef NEW_NV_RECORD_ENALBED

