/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINK_LORA_H
#define LINK_LORA_H

#include <stdbool.h>
#include <stdio.h>

#define LORAWAN_APP_DATA_BUFF_SIZE 242
#define APP_TX_DUTYCYCLE 30000
#define LORAWAN_CONFIRMED_MSG ENABLE
#define LORAWAN_APP_PORT 100
#define JOINREQ_NBTRIALS 3

enum {
    LORAWAN_ADR_OFF = 0,
    LORAWAN_ADR_ON = 1,
};

typedef enum node_mode_s {
    NODE_MODE_NORMAL = 0,
    NODE_MODE_REPEATER = 1,  // Alibaba Node Repeater
    NODE_MODE_AUTOSWITCH = 2,  // switch between normal and repeater
} node_mode_t;

typedef enum node_freq_type_s {
    FREQ_TYPE_INTER = 0,  // uplink and downlink use different frequencies
    FREQ_TYPE_INTRA = 1,  // uplink and downlink use same frequencies
    FREQ_TYPE_MAX,
} node_freq_type_t;

typedef enum {
    VALID_LORA_CONFIG = 0xbeef,
    INVALID_LORA_CONFIG = 0xfffe,
};

typedef struct lora_config_s {
    uint32_t freqband;
    int8_t datarate;
    uint16_t flag;
} lora_config_t;

typedef struct lora_dev_s {
    uint8_t dev_eui[8];
    uint8_t app_eui[8];
    uint8_t app_key[16];
    int8_t class;
    uint8_t mode;  // normal or repeater
    uint16_t flag;
} lora_dev_t;

typedef enum join_method_s {
    STORED_JOIN_METHOD = 0,
    DEF_JOIN_METHOD = 1,
    SCAN_JOIN_METHOD = 2,
    JOIN_METHOD_NUM
} join_method_t;

#ifdef CONFIG_DEBUG_LINKWAN
#include "debug.h"
#define DBG_LINKWAN(...)     PRINTF(__VA_ARGS__)
#else
#define DBG_LINKWAN(...)
#define LORA_LOG(...)
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
    uint8_t (*BoardGetBatteryLevel)(void);
    void (*BoardGetUniqueId)(uint8_t *id);
    uint32_t (*BoardGetRandomSeed)(void);
    void (*LoraTxData)(lora_AppData_t *AppData);
    void (*LoraRxData)(lora_AppData_t *AppData);
} LoRaMainCallback_t;

typedef struct sLoRaParam {
    TxEventType_t TxEvent;
    uint32_t TxDutyCycleTime;
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

node_freq_type_t get_lora_freq_type(void);
bool set_lora_tx_datarate(int8_t datarate);
bool set_lora_tx_dutycycle(uint32_t dutycycle);
bool set_lora_tx_len(uint16_t len);
bool set_lora_tx_confirmed_flag(int confirmed);
bool set_lora_tx_num_trials(uint8_t trials);
bool set_lora_state(DeviceState_t state);
bool send_lora_link_check(void);
bool set_lora_class(int8_t class);

bool set_lora_dev_eui(uint8_t *eui);
bool set_lora_app_eui(uint8_t *eui);
bool set_lora_app_key(uint8_t *key);

#endif /* LINK_LORA_H */
