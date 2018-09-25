/*
 * File      : sd.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SD_H__
#define __SD_H__

#include "mmcsd_host.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t mmcsd_send_if_cond(struct rt_mmcsd_host *host, uint32_t ocr);
int32_t mmcsd_send_app_op_cond(struct rt_mmcsd_host *host, uint32_t ocr, uint32_t *rocr);

int32_t mmcsd_get_card_addr(struct rt_mmcsd_host *host, uint32_t *rca);
int32_t mmcsd_get_scr(struct rt_mmcsd_card *card, uint32_t *scr);

int32_t init_sd(struct rt_mmcsd_host *host, uint32_t ocr);

#ifdef __cplusplus
}
#endif

#endif
