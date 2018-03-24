/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "low_power.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
#include "version.h"
#include "radio.h"
#include "sx1276Regs-Fsk.h"
#include "sx1276Regs-LoRa.h"
#include "delay.h"
#include "hal/lorawan.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*!
 * CAYENNE_LPP is myDevices Application server.
 */

/*!
 * Defines the application data transmission duty cycle. 30s, value in [ms].
 */
#define APP_TX_DUTYCYCLE 30000
/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON 1
/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG ENABLE
/*!
 * LoRaWAN application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_APP_PORT 100
/*!
 * Number of trials for the join request.
 */
#define JOINREQ_NBTRIALS 3 //will be changed to 48 in RegionCN470Verify()

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* call back when LoRa will transmit a frame*/
static void LoraTxData( lora_AppData_t *AppData, FunctionalState *IsTxConfirmed );

/* call back when LoRa has received a frame*/
static void LoraRxData( lora_AppData_t *AppData );

/* Private variables ---------------------------------------------------------*/
/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks = {
    HW_GetBatteryLevel,
    HW_GetUniqueId,
    HW_GetRandomSeed,
    LoraTxData,
    LoraRxData
};

/**
 * Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = {
    TX_ON_TIMER,
    APP_TX_DUTYCYCLE,
    CLASS_A,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int application_start( void )
{
    /* Configure the hardware*/
    HW_Init( );
    
    /* Configure Debug mode */
    DBG_Init( );
    
    /* lorawan interface configuration */

    /* Configure the Lora Stack*/
    lora_Init( &LoRaMainCallbacks, &LoRaParamInit );
    
    PRINTF("FW VERSION: %s\n\r", AT_VERSION_STRING)
    ;
    PRINTF("LoRaWan VERSION: %s\n\r", AT_VERSION_LORAWAN_STRING)
    ;
    
    /* main loop*/
    while ( 1 )
    {
        /* run the LoRa class A state machine*/
        lora_fsm( );
        
        DISABLE_IRQ();
        /* if an interrupt has occurred after DISABLE_IRQ, it is kept pending
         * and cortex will not enter low power anyway  */
        if ( lora_getDeviceState( ) == DEVICE_STATE_SLEEP )
        {
#ifndef LOW_POWER_DISABLE
            LowPower_Handler( );
#endif
        }
        ENABLE_IRQ();
        
    }
}

static void LoraTxData( lora_AppData_t *AppData, FunctionalState *IsTxConfirmed )
{
    AppData->BuffSize = sprintf( (char *) AppData->Buff, "app lora demo" );
    DBG_PRINTF( "tx: %s\n", AppData->Buff );
    
    AppData->Port = LORAWAN_APP_PORT;
    
    *IsTxConfirmed = LORAWAN_CONFIRMED_MSG;
    
}

static void LoraRxData( lora_AppData_t *AppData )
{
    AppData->Buff[AppData->BuffSize] = '\0';
    DBG_PRINTF( "rx: port = %d, len = %d, data = %s\n", AppData->Port, AppData->BuffSize, AppData->Buff );
    switch ( AppData->Port )
    {
        default:
            break;
    }
}
