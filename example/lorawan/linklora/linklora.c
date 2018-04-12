/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hw.h"
#include "low_power.h"
#include "linklora.h"
#include "timeServer.h"
#include "version.h"
#include "radio.h"
#include "sx1276Regs-Fsk.h"
#include "sx1276Regs-LoRa.h"
#include "delay.h"

#include "LoRaMac.h"
#include "Region.h"
#include "commissioning.h"

#define APP_TX_DUTYCYCLE 30000
#define LORAWAN_ADR_ON 1
#define LORAWAN_CONFIRMED_MSG ENABLE
#define LORAWAN_APP_PORT 100
#define JOINREQ_NBTRIALS 3

static void LoraTxData( lora_AppData_t *AppData, int8_t *IsTxConfirmed );
static void LoraRxData( lora_AppData_t *AppData );

static LoRaMainCallback_t LoRaMainCallbacks = {
    HW_GetBatteryLevel,
    HW_GetUniqueId,
    HW_GetRandomSeed,
    LoraTxData,
    LoraRxData
};

static LoRaParam_t LoRaParamInit = {
    TX_ON_TIMER,
    APP_TX_DUTYCYCLE,
    CLASS_A,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS
};

int application_start( void )
{
    HW_Init( );
    DBG_Init( );

    lora_init( &LoRaMainCallbacks, &LoRaParamInit );

    while (1) {
        lora_fsm( );

        DISABLE_IRQ();
        if (lora_getDeviceState( ) == DEVICE_STATE_SLEEP) {
#ifndef LOW_POWER_DISABLE
            LowPower_Handler( );
#endif
        }
        ENABLE_IRQ();
    }
}

static void LoraTxData( lora_AppData_t *AppData, int8_t *IsTxConfirmed )
{
    AppData->BuffSize = sprintf( (char *) AppData->Buff, "linklora demo data" );
    DBG_LINKLORA("tx: %s\r\n", AppData->Buff );

    AppData->Port = LORAWAN_APP_PORT;
    *IsTxConfirmed = LORAWAN_CONFIRMED_MSG;
}

static void LoraRxData( lora_AppData_t *AppData )
{
    AppData->Buff[AppData->BuffSize] = '\0';
    DBG_LINKLORA( "rx: port = %d, len = %d, data = %s\r\n", AppData->Port, AppData->BuffSize, AppData->Buff );
}
