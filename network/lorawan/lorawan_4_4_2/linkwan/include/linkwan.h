/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_H
#define LINKWAN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "linkwan_config.h"

#define JOINREQ_NBTRIALS 3

typedef enum node_mode_s
{
    NODE_MODE_NORMAL     = 0,
    NODE_MODE_REPEATER   = 1, // Alibaba Node Repeater
    NODE_MODE_AUTOSWITCH = 2, // switch between normal and repeater
} node_mode_t;

typedef enum node_freq_mode_s
{
    FREQ_MODE_INTRA = 1, // uplink and downlink use same frequencies
    FREQ_MODE_INTER = 2, // uplink and downlink use different frequencies
} node_freq_mode_t;

typedef enum
{
    VALID_LORA_CONFIG   = 0xbeef,
    INVALID_LORA_CONFIG = 0xfffe,
};


typedef struct lora_dev_s
{
    uint8_t dev_eui[8];
    uint8_t app_eui[8];
    uint8_t app_key[16];
    int8_t  freqband;
    int8_t  datarate;
    int8_t class;
    uint8_t  mode; // normal or repeater
    uint16_t mask;
    uint16_t flag;
    uint16_t crc;
} __attribute__((packed)) lora_dev_t;


typedef struct lora_abp_id_s
{
    uint8_t  devaddr[4];
    uint8_t  appskey[16];
    uint8_t  nwkskey[16];
    uint16_t flag;
} lora_abp_id_t;

typedef enum join_method_s
{
    STORED_JOIN_METHOD = 0,
    DEF_JOIN_METHOD    = 1,
    SCAN_JOIN_METHOD   = 2,
    JOIN_METHOD_NUM
} join_method_t;

typedef enum app_class_type_s
{
    LORA_APP_CLASS_A = 0,
    LORA_APP_CLASS_B,
    LORA_APP_CLASS_C,
} app_class_type_t;

#ifdef CONFIG_DEBUG_LINKWAN
#include "debug.h"
#define DBG_LINKWAN(...) PRINTF(__VA_ARGS__)
#else
#define DBG_LINKWAN(...)
#endif

typedef enum eJoinModeType
{
    JOIN_MODE_OTAA,
    JOIN_MODE_ABP
} JoinMode_t;

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
} __attribute__((packed)) lora_AppData_t;

typedef struct sLoRaMainCallback
{
    uint8_t (*BoardGetBatteryLevel)(void);
    void (*BoardGetUniqueId)(uint8_t *id);
    uint32_t (*BoardGetRandomSeed)(void);
    void (*LoraTxData)(lora_AppData_t *AppData);
    void (*LoraRxData)(lora_AppData_t *AppData);
} __attribute__((packed)) LoRaMainCallback_t;

typedef struct sLoRaParam
{
    TxEventType_t TxEvent;
    uint32_t      TxDutyCycleTime;
    bool          AdrEnable;
    int8_t        TxDatarate;
    bool          EnablePublicNetwork;
    uint8_t       NbTrials;
    JoinMode_t    JoinMode;
} __attribute__((packed)) LoRaParam_t;

typedef enum eDevicState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_JOINED,
    DEVICE_STATE_REQ_DEVICE_TIME,
    DEVICE_STATE_REQ_PINGSLOT_ACK,
    DEVICE_STATE_BEACON_ACQUISITION,
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

bool set_lora_freq_band(int8_t freqband);
void get_lora_freq_band(int8_t *freqband);

node_freq_mode_t get_lora_freq_mode(void);
bool             set_lora_freq_mode(node_freq_mode_t mode);
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

lora_AppData_t *get_lora_tx_data(void);
bool            tx_lora_data(void);
lora_AppData_t *get_lora_rx_data(void);
void            tx_lora_mac_req(void);

int    get_device_status(void);
void   get_lora_rssi(uint8_t band, int16_t *channel_rssi);
bool   get_lora_report_mode(void);
bool   set_lora_report_mode(int8_t mode);
int8_t get_lora_tx_power();
bool   set_lora_tx_power(int8_t pwr);
bool   set_lora_mac_rx1_delay(long rx1delay);
// for linkWAN test
bool    set_lora_tx_len(uint16_t len);
uint8_t get_lora_tx_len(void);

bool send_lora_link_check(void);

bool lora_tx_data_payload(uint8_t confirm, uint8_t Nbtrials, uint8_t *payload,
                          uint8_t len);
bool set_lora_app_port(uint8_t port);

void set_classb_ping_period(uint8_t ping_period);

int8_t multicast_add(uint32_t dev_addr, uint32_t frequency, uint8_t  data_rate, uint16_t periodicity, uint8_t *mc_key );

int8_t multicast_delete(uint32_t dev_addr);

uint8_t multicast_get_num(void);

#endif /* LINKWAN_H */
