/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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