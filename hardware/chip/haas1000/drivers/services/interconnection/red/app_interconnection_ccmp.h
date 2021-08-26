/**
 * @file app_interconnection_ccmp.h
 * 
 * @brief definations of ccmp service of HUAWEI
 * 
 * Copyright 2015-2018 BES.
 * All rights reserved. All unpublished rights reserved.
 * 
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 * 
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 * 
 */
#ifdef __INTERCONNECTION__
#ifndef __APP_INTERCONNECTION_CCMP_DEFINE_H__
#define __APP_INTERCONNECTION_CCMP_DEFINE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "bluetooth.h"

#define CCMP_RECV_BUFFER_SIZE                        (4096+100)

struct spp_device *app_interconnection_get_ccmp_dev();

void app_interconnection_send_data_via_ccmp(uint8_t *ptrData, uint32_t length);

void app_ccmp_client_open(uint8_t *pServiceSearchRequest, uint8_t serviceSearchRequestLen, uint8_t port);

void app_interconnection_ccmp_disconnected_callback();

#ifdef __cplusplus
}
#endif

#endif // #ifndef __APP_INTERCONNECTION_CCMP_DEFINE_H__
#endif // #ifdef __INTERCONNECTION__