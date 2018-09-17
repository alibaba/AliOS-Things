/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hw.h"
#include "low_power.h"
#include "linkwan.h"
#include "timer.h"
#include "version.h"
#include "radio.h"
#include "delay.h"

#include "LoRaMac.h"
#include "Region.h"
#include "commissioning.h"
#include "lorawan_port.h"

#include <k_api.h>

static void LoraTxData(lora_AppData_t *AppData)
{
    uint8_t index = 0;

    AppData->BuffSize = get_lora_tx_len();
    if (AppData->BuffSize < 1) {
        AppData->BuffSize = 1;
    }

    for (index = 0; index < AppData->BuffSize; index++) {
        AppData->Buff[index] = '0' + index;
    }
    AppData->Port = 100;
    DBG_LINKWAN("tx, port %d, size %d\r\n", AppData->Port, AppData->BuffSize);
}

static void LoraRxData(lora_AppData_t *AppData)
{
    if (AppData->BuffSize > 0) {
        DBG_LINKWAN( "rx, port %d, size %d\r\n", AppData->Port, AppData->BuffSize);
    }
}

static LoRaMainCallback_t LoRaMainCallbacks;

static void lora_task_entry(void *arg)
{
    LoRaMainCallbacks.BoardGetBatteryLevel = aos_lrwan_sys.get_battery_level;
    LoRaMainCallbacks.BoardGetUniqueId = aos_lrwan_sys.get_unique_id;
    LoRaMainCallbacks.BoardGetRandomSeed = aos_lrwan_sys.get_random_seed;
    LoRaMainCallbacks.LoraTxData = LoraTxData;
    LoRaMainCallbacks.LoraRxData = LoraRxData;

    lora_init(&LoRaMainCallbacks);
    lora_fsm();
}

int application_start( void )
{
    lora_task_entry(NULL);
}
