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
#include "lorawan.h"
#include "commissioning.h"
#include "lorawan_port.h"
#include <k_api.h>


static LoRaMainCallback_t LoRaMainCallbacks;
extern hal_lrwan_sys_t aos_lrwan_sys;


/*
 * Function Name: LoRaTxData
 * Function Discription: The Callback of LoRa Tx, called by lorawan stack
 *                       when send data to server.
 * Input Param:  AppData
 * Output Param: None
 * Return:       None
 */
static void LoraTxData(lora_AppData_t *AppData)
{
    uint8_t i = 0;

    for (i = 0; i < AppData->BuffSize; i++) {
        AppData->Buff[i] = '0' + i;
    }

    /* Print TX Info Messages */
    DBG_LINKWAN("tx, port %d, size %d\r\n", AppData->Port, AppData->BuffSize);
}


/*
 * Function Name: LoRaRxData
 * Function Discription: The Callback of LoRa Rx, called by lorawan stack
 *                       when received data from server.
 * Input Param:  AppData
 * Output Param: None
 * Return:       None
 */
static void LoraRxData(lora_AppData_t *AppData)
{
    /* Print RX Info Messages */
    if (AppData->BuffSize > 0) {
        DBG_LINKWAN( "rx, port %d, size %d\r\n", AppData->Port, AppData->BuffSize);
    }
}


/*
 * Function Name: lora_task_entry
 * Function Discription: The Entry Function of LoRaWAN Stack
 * Input Param:  void *arg
 * Output Param: None
 * Return:       None
 */
static void lora_task_entry(void *arg)
{
    /* Register the Callbacks of LoRaWAN APP
     * To be modified by user to for the specific application*/
    LoRaMainCallbacks.BoardGetBatteryLevel = aos_lrwan_sys.get_battery_level;
    LoRaMainCallbacks.BoardGetUniqueId = aos_lrwan_sys.get_unique_id;
    LoRaMainCallbacks.BoardGetRandomSeed = aos_lrwan_sys.get_random_seed;
    LoRaMainCallbacks.LoraTxData = LoraTxData;
    LoRaMainCallbacks.LoraRxData = LoraRxData;

    /* Initialize the LoRaWAN Stack */
    lora_init(&LoRaMainCallbacks, LORA_APP_CLASS_A);

    /* Start the FSM of LoRaWAN Stark, main loop inside,
     * it is the core engine of LoRaWAN Stack */
    lora_fsm();
}


/*
 * Function Name: application_start
 * Function Discription: The Entry Function of LoRaWAN Application
 * Input Param:  int argc
 *               char **argv
 * Output Param: None
 * Return:       None
 */
int application_start(int argc, char **argv)
{
    lora_task_entry(NULL);
}
