/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef GATEWAY_SERVICE_H
#define GATEWAY_SERVICE_H

#ifndef bool
#define bool unsigned char
#endif

int gateway_service_init(void);
void gateway_service_deinit(void);
int gateway_service_start(void);
void gateway_service_stop(void);
bool gateway_service_get_mesh_mqtt_state(void);

#endif
