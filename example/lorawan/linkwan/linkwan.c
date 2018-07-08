/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hw.h"
#include "low_power.h"
#include "linkwan.h"
#include "timeServer.h"
#include "version.h"
#include "radio.h"
#include "sx1276Regs-Fsk.h"
#include "sx1276Regs-LoRa.h"
#include "delay.h"

#include "LoRaMac.h"
#include "Region.h"
#include "commissioning.h"

#include <k_api.h>

static void LoraTxData(lora_AppData_t *AppData);
static void LoraRxData(lora_AppData_t *AppData);
uint32_t g_msg_index = 0;

static LoRaMainCallback_t LoRaMainCallbacks = {
    HW_GetBatteryLevel,
    HW_GetUniqueId,
    HW_GetRandomSeed,
    LoraTxData,
    LoraRxData
};

static void lora_task_entry(void *arg)
{
    lora_init(&LoRaMainCallbacks);
    set_lora_tx_dutycycle(30000);
    lora_fsm();
}

ktask_t g_lora_task;
cpu_stack_t g_lora_task_stack[512];
int application_start( void )
{
    krhino_task_create(&g_lora_task, "lora_task", NULL,
                       5, 0u, g_lora_task_stack,
                       512, lora_task_entry, 1u);
}

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
    DBG_LINKWAN("tx, port %d, size %d, index %d\r\n", AppData->Port, AppData->BuffSize, g_msg_index++);
}

static void LoraRxData(lora_AppData_t *AppData)
{
    if (AppData->BuffSize > 0) {
        DBG_LINKWAN( "rx, port %d, size %d\r\n", AppData->Port, AppData->BuffSize);
    }
}
