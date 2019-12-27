/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_H
#define LINKWAN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "linkwan_config.h"

/**
 * Join Request max retry times
 */
#define JOINREQ_NBTRIALS 3

/**
 * LoRa Device Work Mode Enumeration
 */
typedef enum node_mode_s
{
    NODE_MODE_NORMAL     = 0,   /* LoRa Device work in Normal Mode */
    NODE_MODE_REPEATER   = 1,   /* LoRa Device work in Repeater Mode */
    NODE_MODE_AUTOSWITCH = 2,   /* LoRa Device switch between normal and repeater */
} node_mode_t;

/**
 * LoRa Device Freq Mode Enumeration
 */
typedef enum node_freq_mode_s
{
    FREQ_MODE_INTRA = 1,        /* uplink and downlink use same frequencies */
    FREQ_MODE_INTER = 2,        /* uplink and downlink use different frequencies */
} node_freq_mode_t;

/**
 * LoRa Device Configuration Valid Enumeration
 */
typedef enum
{
    VALID_LORA_CONFIG   = 0xbeef,   /* LoRa Device Config Info is Valid */
    INVALID_LORA_CONFIG = 0xfffe,   /* LoRa Device Config Info is Invalid */
};

/**
 * LoRa Device Join Method Enumeration
 */
typedef enum join_method_s
{
    STORED_JOIN_METHOD = 0,     /* Join use Stored Info Method */
    DEF_JOIN_METHOD    = 1,     /* Join use Default Configuration */
    SCAN_JOIN_METHOD   = 2,     /* Join use Scan Freq Method */
    JOIN_METHOD_NUM
} join_method_t;

/**
 * LoRa Device CLASS Type Enumeration
 */
typedef enum app_class_type_s
{
    LORA_APP_CLASS_A = 0,       /* LoRa Type of Class A */
    LORA_APP_CLASS_B,           /* LoRa Type of Class B */
    LORA_APP_CLASS_C,           /* LoRa Type of Class C */
} app_class_type_t;

/**
 * LoRa Device Join Mode Enumeration
 */
typedef enum eJoinModeType
{
    JOIN_MODE_OTAA,         /* Join in OTA Mode*/
    JOIN_MODE_ABP           /* Join in ABP Mode*/
} JoinMode_t;

/**
 * LoRa Device Send Data Trigger Event Type
 */
typedef enum eTxEventType
{
    TX_ON_TIMER,            /* Send Data Triggered by Timer */
    TX_ON_EVENT,            /* Send Data Triggered by Event */
    TX_ON_NONE
} TxEventType_t;

/**
 * LoRa Device State
 */
typedef enum eDevicState
{
    DEVICE_STATE_INIT,                  /* Device is initialized */
    DEVICE_STATE_JOIN,                  /* Device is joining in Network */
    DEVICE_STATE_JOINED,                /* Device is joined */
    DEVICE_STATE_REQ_DEVICE_TIME,       /* Device is Request Device Time, used in Class B */
    DEVICE_STATE_REQ_PINGSLOT_ACK,      /* Device is Request Pingslot ACK, used in Class B */
    DEVICE_STATE_BEACON_ACQUISITION,    /* Device is Searching Beacon, used in Class B */
    DEVICE_STATE_SEND,                  /* Device is Sending Data */
    DEVICE_STATE_SEND_MAC,              /* Device is Sending Mac Data */
    DEVICE_STATE_CYCLE,                 /* Device is Sending Data in Cycle */
    DEVICE_STATE_SLEEP                  /* Device is Sleeping */
} DeviceState_t;

/**
 * LoRa Device Info Struct
 */
typedef struct lora_dev_s
{
    uint8_t dev_eui[8];     /* LoRa Device DEV EUI */
    uint8_t app_eui[8];     /* LoRa Device APP EUI */
    uint8_t app_key[16];    /* LoRa Device APP KEY */
    int8_t  freqband;       /* LoRa Device Freqband Info */
    int8_t  datarate;       /* LoRa Device Data Rate */
    int8_t class;           /* Class Type of LoRa Device used */
    uint8_t  mode;          /* LoRa Mode: normal or repeater */
    uint16_t mask;          /* Band mask of Device */
    uint16_t flag;          /* Valid Flag, valid or invalid */
    uint16_t crc;           /* Crc check of Device Info */
} __attribute__((packed)) lora_dev_t;

/**
 * LoRa Device Info when in APB Mode
 */
typedef struct lora_abp_id_s
{
    uint8_t  devaddr[4];    /* Device Address */
    uint8_t  appskey[16];   /* APPSKEY of LoRa Device */
    uint8_t  nwkskey[16];   /* NETWORK SKEY of LoRa Device */
    uint16_t flag;          /* Valid Flag of APB Info */
} lora_abp_id_t;


#ifdef CONFIG_DEBUG_LINKWAN
#include "debug.h"
#define DBG_LINKWAN(...) PRINTF(__VA_ARGS__)
#else
#define DBG_LINKWAN(...)
#endif

/**
 * LoRa Device Send App Data Struct
 */
typedef struct
{
    uint8_t *Buff;          /* Data Buffer To Send */
    uint8_t  BuffSize;      /* Data Size To Send */
    uint8_t  Port;          /* Tx Port For this LoRa Application */
} __attribute__((packed)) lora_AppData_t;

/**
 * Callbacks for LoRa Application
 */
typedef struct sLoRaMainCallback
{
    uint8_t (*BoardGetBatteryLevel)(void);          /* Get Battery Level Callback */
    void (*BoardGetUniqueId)(uint8_t *id);          /* Get UniqueID Callback */
    uint32_t (*BoardGetRandomSeed)(void);           /* Get Random Seed Callback */
    void (*LoraTxData)(lora_AppData_t *AppData);    /* Send Data Complete Callback */
    void (*LoraRxData)(lora_AppData_t *AppData);    /* Receive Data Callback */
} __attribute__((packed)) LoRaMainCallback_t;

/**
 * LoRaWAN Network Paramters
 */
typedef struct sLoRaParam
{
    TxEventType_t TxEvent;                  /* TX Trigger Type: Timer or Event */
    uint32_t      TxDutyCycleTime;          /* TX Dutycycle Time: valid when TX is triggered by timer */
    bool          AdrEnable;                /* Adaptive Datarate Enable Flag */
    int8_t        TxDatarate;               /* TX Datarate */
    bool          EnablePublicNetwork;      /* Public Network is Enabled Flag */
    uint8_t       NbTrials;                 /* TX Retry Times when Send failed */
    JoinMode_t    JoinMode;                 /* Join Mode: OTAA or ABP */
} __attribute__((packed)) LoRaParam_t;

/**
 * set LoRa Device Freqband Mask
 *
 * @param[in]  mask         freqband mask info
 *
 * @return true on success
 */
bool set_lora_freqband_mask(uint16_t mask);

/**
 * get LoRa Device Freqband Mask
 *
 * @param none
 *
 * @return freqband mask value
 */
uint16_t get_lora_freqband_mask(void);

/**
 * set LoRa Device EUI
 *
 * @param[in]  eui      LoRa Device EUI
 *
 * @return true when success
 */
bool set_lora_dev_eui(uint8_t *eui);

/**
 * get LoRa Device EUI
 *
 * @param[out]  eui      LoRa Device EUI
 *
 * @return  none
 */
void get_lora_dev_eui(uint8_t *eui);

/**
 * set LoRa App EUI
 *
 * @param[in]  eui      LoRa App EUI
 *
 * @return true when success
 */
bool set_lora_app_eui(uint8_t *eui);

/**
 * get LoRa App EUI
 *
 * @param[out]  eui      LoRa App EUI
 *
 * @return  none
 */
void get_lora_app_eui(uint8_t *eui);

/**
 * set LoRa App KEY
 *
 * @param[in]  key      LoRa App KEY
 *
 * @return true when success
 */
bool set_lora_app_key(uint8_t *key);

/**
 * get LoRa App KEY
 *
 * @param[out]  key      LoRa App KEY
 *
 * @return  none
 */
void get_lora_app_key(uint8_t *key);

/**
 * set LoRa Freq Band
 *
 * @param[in]  freqband      LoRa Device Freqband
 *
 * @return true when success
 */
bool set_lora_freq_band(int8_t freqband);

/**
 * get LoRa Freq Band
 *
 * @param[out]  freqband      LoRa Device Freqband
 *
 * @return  none
 */
void get_lora_freq_band(int8_t *freqband);

/**
 * set LoRa Freq Mode, Inter Freq or Intra Freq
 *
 * @param[in]  Freq Mode
 *
 * @return true when success, false when failed
 */
bool set_lora_freq_mode(node_freq_mode_t mode);

/**
 * get LoRa Freq Mode, Inter Freq or Intra Freq
 *
 * @param  none
 *
 * @return  FREQ_MODE_INTRA: Intra Freq Mode
 *          FREQ_MODE_INTER: Inter Freq Mode
 */
node_freq_mode_t get_lora_freq_mode(void);

/**
 * set LoRa Device TX Datarate
 *
 * @param[in]  datarate    LoRa Device TX Datarate
 *
 * @return true when success
 */
bool set_lora_tx_datarate(int8_t datarate);

/**
 * get LoRa Device TX Datarate
 *
 * @param none
 *
 * @return LoRa Device TX Datarate
 */
int8_t get_lora_tx_datarate(void);

/**
 * Enable or Disable LoRa Device Adaptive Datarate
 *
 * @param[in]  state   Enable or Disable ADR
 *
 * @return true when success
 */
bool set_lora_adr(int state);

/**
 * get LoRa Device Adaptive Datarate State
 *
 * @param[in]  none
 *
 * @return 1 when ADR is enabled, 0 when ADR is disabled
 */
int get_lora_adr(void);

/**
 * set LoRa Device Adaptive Datarate State
 *
 * @param[in]  class   Class Type of LoRa Device
 *
 * @return true when success
 */
bool set_lora_class(int8_t class);

/**
 * get LoRa Device Class Type
 *
 * @param[in]  none
 *
 * @return class type of LoRa Device
 */
int8_t get_lora_class(void);

/**
 * set LoRa TX Confirmed Data Type Flag
 *
 * @param[in] confirmed   Confirmed Data Type Flag
 *
 * @return true when success
 */
bool set_lora_tx_cfm_flag(int confirmed);

/**
 * get LoRa TX Confirmed Data Type Flag
 *
 * @param[in] none
 *
 * @return 1 when Confirmed Flag is set
 */
int get_lora_tx_cfm_flag(void);

/**
 * set LoRa TX Retry Times when Send Failed in Confirmed Mode
 *
 * @param[in] trials   Retry Times of Sendind Confirmed Data
 *
 * @return true when success
 */
bool set_lora_tx_cfm_trials(uint8_t trials);

/**
 * get LoRa TX Retry Times when Send Failed in Confirmed Mode
 *
 * @param[in] none
 *
 * @return retry time of sending confirmed data
 */
uint8_t get_lora_tx_cfm_trials(void);

/**
 * set LoRa Device State
 *
 * @param[in] state   LoRa Device State
 *
 * @return true when success
 */
bool set_device_state(DeviceState_t state);

/**
 * get LoRa Device State
 *
 * @param[in] none
 *
 * @return LoRa Device Status
 */
int get_device_state(void);

/**
 * set LoRa Device TX Dutycycle
 *
 * @param[in] dutycycle  the dutycycle of sending data in cycle mode
 *
 * @return true when success
 */
bool set_lora_tx_dutycycle(uint32_t dutycycle);

/**
 * get LoRa Device TX Dutycycle
 *
 * @param[in] none
 *
 * @return the dutycycle of sending data in cycle mode
 */
uint32_t get_lora_tx_dutycycle(void);


/**
 * get LoRa Device TX Data Info, including data buffer, data length, application port
 *
 * @param[in] none
 *
 * @return LoRa Device TX Data Info
 */
lora_AppData_t *get_lora_tx_data(void);

/**
 * trigger LoRa Device to Send Data in Buffer
 *
 * @param[in] none
 *
 * @return true when success
 */
bool tx_lora_data(void);

/**
 * get LoRa Device RX Data Info, including data buffer, data length, application port
 *
 * @param[in] none
 *
 * @return LoRa Device RX Data Info
 */
lora_AppData_t *get_lora_rx_data(void);

/**
 * trigger LoRa Device to Send Mac Data in Buffer
 *
 * @param[in] none
 *
 * @return none
 */
void tx_lora_mac_req(void);

/**
 * get RSSI of LoRa Device
 *
 * @param[in]   band           the Band Info of LoRa Device
 *  param[out]  channel_rssi   RSSI of LoRa Device
 *  param[out]  channel_snr    SNR of LoRa Device
 *
 * @return none
 */
void get_lora_rssi(uint8_t band, int16_t *channel_rssi, int8_t *channel_snr);

/**
 * set RSSI of LoRa Device
 *
 * @param[in]   band           the index of LoRa Channel
 *  param[in]   channel_rssi   RSSI of LoRa Device
 *  param[in]   channel_snr    SNR of LoRa Device
 *
 * @return none
 */
void set_lora_rssi(uint8_t index, int16_t channel_rssi, int8_t channel_snr);

/**
 * get Report Mode of LoRa Device
 *
 * @param[in] none
 *
 * @return true when success
 */
bool get_lora_report_mode(void);

/**
 * set Report Mode of LoRa Device
 *
 * @param[in] mode  Report Mode
 *
 * @return true when success
 */
bool set_lora_report_mode(int8_t mode);

/**
 * get TX Power of LoRa Device
 *
 * @param[in] none
 *
 * @return TX Power of LoRa Device
 */
int8_t get_lora_tx_power();

/**
 * set TX Power of LoRa Device
 *
 * @param[in] pwr TX Power of LoRa Device
 *
 * @return true when success
 */
bool set_lora_tx_power(int8_t pwr);

/**
 * set rx1 window delay of LoRa Device
 *
 * @param[in] rx1delay    rx1 window delay of LoRa Device
 *
 * @return true when success
 */
bool set_lora_mac_rx1_delay(long rx1delay);

/**
 * set TX Data Length
 *
 * @param[in] len   Data Length to be Sent
 *
 * @return true when success
 */
bool set_lora_tx_len(uint16_t len);

/**
 * get TX Data Length
 *
 * @param[in] len   Data Length to be Sent
 *
 * @return Data Length to be Sent
 */
uint8_t get_lora_tx_len(void);

/**
 * Trigger to send LoRa Link Check Request to Network Server
 *
 * @param[in] none
 *
 * @return true when success
 */
bool send_lora_link_check(void);

/**
 * Set LoRa TX Payload when need to Send Data
 *
 * @param[in] confirm   whether data is confirmed or unconfirmed
 *  param[in] Nbtrials  Retry times of TX
 *  param[in] payload   data buffer to be sent
 *  param[in] len       data length to be sent
 *
 * @return true when success
 */
bool lora_tx_data_payload(uint8_t confirm, uint8_t Nbtrials, uint8_t *payload,
                          uint8_t len);

/**
 * Set LoRa Application Port
 *
 * @param[in] port  LoRa Application Port
 *
 * @return true when success
 */
bool set_lora_app_port(uint8_t port);

/**
 * Set LoRa Ping Period when in Class B Mode
 *
 * @param[in] ping_period   Ping Period when in Class B Mode
 *
 * @return none
 */
void set_classb_ping_period(uint8_t ping_period);

/**
 * Add Multicast Group
 *
 * @param[in] dev_addr     group addr of multicast
 *  param[in] frequency    frequency of multicast
 *  param[in] data_rate    data rate of multicast
 *  param[in] periodicity  periodicity of multicast
 *  param[in] mc_key       multicast key
 *
 * @return none
 */
int8_t multicast_add(uint32_t dev_addr, uint32_t frequency, uint8_t  data_rate, uint16_t periodicity, uint8_t *mc_key );

/**
 * Delete Multicast Group
 *
 * @param[in] dev_addr     group addr of multicast
 *
 * @return none
 */
int8_t multicast_delete(uint32_t dev_addr);

/**
 * Get the number of multicast
 *
 * @param[in] none
 *
 * @return number of multicast
 */
uint8_t multicast_get_num(void);

#endif /* LINKWAN_H */

