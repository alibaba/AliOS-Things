/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "board.h"
#include "radio.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"
#include "sx126x-board.h"
#include "aos/hal/timer.h"
#include "ulog/ulog.h"

#define TRANS_MODE_TX        (0xEF)
#define TRANS_MODE_RX        (0xFE)

// set transmission mode here, TRANS_MODE_TX or TRANS_MODE_RX
#define TRANSMIT_MODE TRANS_MODE_TX

#define RF_FREQUENCY                                470000000 // 779000000 Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              2         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define BUFFER_SIZE                                 64 // Define the payload size here
uint8_t g_buffer[BUFFER_SIZE] = {0};

static RadioEvents_t g_RadioCallback;

void OnTxDone( ) {
    LOG("[lora demo][%s] << %s \n", __func__, g_buffer);
    memset(g_buffer, 0, BUFFER_SIZE);
    sleep(5);
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ) {
    LOG("[lora demo][%s] >> %s \n", __func__, payload);
    Radio.Sleep();
    Radio.Rx( 1000 );
}

/**
 * Main application entry point.
 */
int application_start(int argc, char *argv[]) {
    bool isMaster = true;
    uint8_t i;

    // set log level
    aos_set_log_level(AOS_LL_DEBUG);

    // Radio initialization
    g_RadioCallback.TxDone = OnTxDone;
    g_RadioCallback.RxDone = OnRxDone;
    Radio.Init( &g_RadioCallback );
    LOG("[lora demo] ++init radio done++");

    Radio.SetChannel( RF_FREQUENCY );

#if defined(TRANSMIT_MODE) && (TRANSMIT_MODE == TRANS_MODE_TX)
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    LOG("[lora demo] ++set TX config done++");
#elif defined(TRANSMIT_MODE) && (TRANSMIT_MODE == TRANS_MODE_RX)
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
    LOG("[lora demo] ++set RX config done++");
#else
    #error "PLEASE SET VALID TRANSMIT MODE FIRST!!"
#endif

    Radio.SetMaxPayloadLength( MODEM_LORA, BUFFER_SIZE );
    LOG("[lora demo] ++lora init done++\n");

#if defined(TRANSMIT_MODE) && (TRANSMIT_MODE == TRANS_MODE_RX)
    Radio.Rx( 1000 );
#endif

    while( 1 )
    {
#if defined(TRANSMIT_MODE) && (TRANSMIT_MODE == TRANS_MODE_TX)
        if(strlen(g_buffer) == 0) {
            snprintf(g_buffer, BUFFER_SIZE, "Hello from HaaS@%llu...", aos_now_ms());
            Radio.Send( g_buffer, strlen(g_buffer)+1);
        }
#endif
        if( Radio.IrqProcess != NULL )
        {
            Radio.IrqProcess( );
        }
        // sleep for other system task, such as CLI task.
        aos_msleep(100);
    }
    return 0;
}
