/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINK_LORA_H
#define LINK_LORA_H

#include <stdbool.h>
#include <stdio.h>

typedef enum node_mode_s {
    NODE_MODE_NORMAL = 0,
    NODE_MODE_REPEATER = 1,  // Alibaba Node Repeater
    NODE_MODE_AUTOSWITCH = 2,  // switch between normal and repeater
} node_mode_t;

typedef enum node_freq_type_s {
    FREQ_TYPE_INTER = 0,  // uplink and downlink use different frequencies
    FREQ_TYPE_INTRA = 1,  // uplink and downlink use same frequencies
} node_freq_type_t;

typedef enum {
    VALID_LORA_CONFIG = 0xbeef,
    INVALID_LORA_CONFIG = 0xffff,
};

typedef struct lora_config_s {
    uint32_t freqband;
    int8_t datarate;
    uint8_t mode;  // normal or repeater
    uint16_t flag;
} lora_config_t;

typedef enum join_method_s {
    STORED_JOIN_METHOD = 0,
    DEF_JOIN_METHOD = 1,
    SCAN_JOIN_METHOD = 2,
    JOIN_METHOD_NUM
} join_method_t;

#ifdef CONFIG_DEBUG_LINKLORA
#include "debug.h"
#define DBG_LINKLORA(...)     PRINTF(__VA_ARGS__)
#else
#define DBG_LINKLORA(...)
#endif

typedef enum eTxEventType {
/*!
 * @brief AppdataTransmition issue based on timer every TxDutyCycleTime
 */
    TX_ON_TIMER,
/*!
 * @brief AppdataTransmition external event plugged on OnSendEvent( )
 */
    TX_ON_EVENT
} TxEventType_t;

typedef struct {
  uint8_t* Buff;
  uint8_t BuffSize;
  uint8_t Port;
} lora_AppData_t;

typedef struct sLoRaMainCallback {
    uint8_t ( *BoardGetBatteryLevel )( void );
    void    ( *BoardGetUniqueId ) ( uint8_t *id);
    uint32_t ( *BoardGetRandomSeed ) (void);
    void ( *LoraTxData ) ( lora_AppData_t *AppData, int8_t* IsTxConfirmed);
    void ( *LoraRxData ) ( lora_AppData_t *AppData);
} LoRaMainCallback_t;

typedef struct sLoRaParam {
    TxEventType_t TxEvent;
    uint32_t TxDutyCycleTime;
    int8_t Class;
    bool AdrEnable;
    int8_t TxDatarate;
    bool EnablePublicNetwork;
    uint8_t NbTrials;
} LoRaParam_t;

typedef enum eDevicState {
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_JOINED,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
} DeviceState_t;

#endif /* LINK_LORA_H */
