/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _TAG_BLE_OTA_
#define _TAG_BLE_OTA_

#include <stdint.h>

typedef enum {
    OTA_ST_IDLE = 0,
    OTA_ST_START,
    OTA_ST_REQUEST,
    OTA_ST_DL,
    OTA_ST_FLASH,
    OTA_ST_STOP,
    OTA_ST_COMPLETE,
} ota_state_en;

typedef void (*ota_event_callback_t)(ota_state_en ota_state);

int ble_ota_init(ota_event_callback_t cb);

void ble_ota_process();

#endif /* _TAG_BLE_OTA_ */

/******************************** END OF FILE *********************************/
