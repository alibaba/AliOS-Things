/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hw.h"
#include "low_power.h"
#include "linkwan.h"
#include "system/timer.h"
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
    uint8_t i = 0;

    for (i = 0; i < AppData->BuffSize; i++) {
        AppData->Buff[i] = '0' + i;
    }
    AppData->Port = 10;
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

    lora_init(&LoRaMainCallbacks, LORA_APP_CLASS_A);
    lora_fsm();
}

int application_start(int argc, char **argv)
{
    lora_task_entry(NULL);
}
