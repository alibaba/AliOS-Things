/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

