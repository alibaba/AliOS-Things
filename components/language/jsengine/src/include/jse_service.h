/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_SERVICE_H
#define JSE_SERVICE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "board_mgr.h"

int dbg_ssdp_start(char* localAddress);

/* websocket report frame */
int bone_websocket_send_frame(char* topic, int level, char* msg);


#if defined(__cplusplus)
}
#endif

#endif /* JSE_SERVICE_H */
