/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_SERVICE_H
#define BE_SERVICE_H

#if defined(__cplusplus)
extern "C" {
#endif

int be_debuger_ssdp_start(char* localAddress);

/* websocket上报信息 */
int bone_websocket_send_frame(char* topic, int level, char* msg);

#if defined(__cplusplus)
}
#endif

#endif /* BE_SERVICE_H */
