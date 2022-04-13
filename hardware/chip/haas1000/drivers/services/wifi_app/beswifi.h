/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef BESWIFI_H
#define BESWIFI_H

#ifdef __cplusplus
extern "C" {
#endif


int bes_wifi_init(void);

int bes_wifi_connect(const char *ssid, const char *passwd, unsigned char *bssid);


#ifdef __cplusplus
}
#endif
#endif /* BESWIFI_H */
