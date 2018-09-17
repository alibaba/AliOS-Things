/*
 * Copyright © 2018 alibaba. All rights reserved.
 */
 
#ifndef MODULE_WIFI_H
#define MODULE_WIFI_H
#if defined(__cplusplus)
extern "C" {
#endif

#include "be_jse.h"

/*type define*/
#define MAX_SCAN_NUM 10
#define WIFI_SCAN_SIZE 128

/*functions declare*/
void module_wifi_load(void);
void module_wifi_reload(void);

#if defined(__cplusplus)
}
#endif
#endif
