/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_H
#define LINKWAN_H

#include <stdbool.h>
#include <stdio.h>

#include "linkwan_config.h"

#define JOINREQ_NBTRIALS 3

typedef enum node_mode_s
{
    NODE_MODE_NORMAL     = 0,
    NODE_MODE_REPEATER   = 1, // Alibaba Node Repeater
    NODE_MODE_AUTOSWITCH = 2, // switch between normal and repeater
} node_mode_t;

typedef enum node_freq_type_s
{
    FREQ_TYPE_INTER = 0, // uplink and downlink use different frequencies
    FREQ_TYPE_INTRA = 1, // uplink and downlink use same frequencies
    FREQ_TYPE_MAX,
} node_freq_type_t;

typedef enum
{
    VALID_LORA_CONFIG   = 0xbeef,
    INVALID_LORA_CONFIG = 0xfffe,
};

typedef struct lora_config_s
{
    uint32_t freqband;
    int8_t   datarate;
    uint16_t flag;
} lora_config_t;

typedef struct lora_dev_s
{
    uint8_t dev_eui[8];
    uint8_t app_eui[8];
    uint8_t app_key[16];
    int8_t class;
    uint8_t  mode; // normal or repeater
    uint16_t mask;
    uint16_t flag;
} lora_dev_t;

typedef enum join_method_s
{
    STORED_JOIN_METHOD = 0,
    DEF_JOIN_METHOD    = 1,
    SCAN_JOIN_METHOD   = 2,
    JOIN_METHOD_NUM
} join_method_t;

#ifdef CONFIG_DEBUG_LINKWAN
#include "debug.h"
#define DBG_LINKWAN(...) PRINTF(__VA_ARGS__)
#else
#define DBG_LINKWAN(...)
#define LORA_LOG(...)
#endif

typedef enum eTxEventType
{
    TX_ON_TIMER,
    TX_ON_EVENT,
    TX_ON_NONE
} TxEventType_t;

typedef struct
{
    uint8_t *Buff;
    uint8_t  BuffSize;
    uint8_t  Port;
} lora_AppData_t;

typedef struct sLoRaMainCallback
{
    uint8_t (*BoardGetBatteryLevel)(void);
    void (*BoardGetUniqueId)(uint8_t *id);
    uint32_t (*BoardGetRandomSeed)(void);
    void (*LoraTxData)(lora_AppData_t *AppData);
    void (*LoraRxData)(lora_AppData_t *AppData);
} LoRaMainCallback_t;

typedef struct sLoRaParam
{
    TxEventType_t TxEvent;
    uint32_t      TxDutyCycleTime;
    bool          AdrEnable;
    int8_t        TxDatarate;
    bool          EnablePublicNetwork;
    uint8_t       NbTrials;
} LoRaParam_t;

typedef enum eDevicState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_JOINED,
    DEVICE_STATE_SEND,
    DEVICE_STATE_SEND_MAC,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
} DeviceState_t;

bool     set_lora_freqband_mask(uint16_t mask);
uint16_t get_lora_freqband_mask(void);

bool set_lora_dev_eui(uint8_t *eui);
bool set_lora_app_eui(uint8_t *eui);
bool set_lora_app_key(uint8_t *key);

node_freq_type_t get_lora_freq_type(void);
bool             set_lora_tx_datarate(int8_t datarate);
int8_t           get_lora_tx_datarate(void);

bool set_lora_adr(int state);
int  get_lora_adr(void);

bool   set_lora_class(int8_t class);
int8_t get_lora_class(void);

bool set_lora_tx_cfm_flag(int confirmed);
int  get_lora_tx_cfm_flag(void);

bool    set_lora_tx_cfm_trials(uint8_t trials);
uint8_t get_lora_tx_cfm_trials(void);

bool set_lora_state(DeviceState_t state);

bool     set_lora_tx_dutycycle(uint32_t dutycycle);
uint32_t get_lora_tx_dutycycle(void);

bool            tx_lora_data(void);
lora_AppData_t *get_lora_data(void);
void            tx_lora_mac_req(void);

// for linkWAN test
bool    set_lora_tx_len(uint16_t len);
uint8_t get_lora_tx_len(void);

bool send_lora_link_check(void);

#endif /* LINKWAN_H */
