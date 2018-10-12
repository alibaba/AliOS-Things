/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_AT_H
#define LINKWAN_AT_H

#define LORA_AT_HELP "AT+?"  // help
#define LORA_AT_RM "AT+RM"  // rm stored lora info

#define LORA_AT_APPEUI "AT+APPEUI"  // app eui
#define LORA_AT_APPKEY "AT+APPKEY"  // app key
#define LORA_AT_DEUI "AT+DEUI"  // device eui

#define LORA_AT_DR "AT+DR"  // datarate
#define LORA_AT_ADR "AT+ADR"  // ADR
#define LORA_AT_CLASS "AT+CLASS"  // class
#define LORA_AT_SCANMASK "AT+SMASK"  // scan mask

#define LORA_AT_CFM "AT+CFM"  // confirmation mode
#define LORA_AT_CFMTRIALS "AT+CTRIALS"  // cfm trials
#define LORA_AT_JOIN "AT+JOIN"  // join network

#define LORA_AT_DCS "AT+DCS"  // dutycycle
#define LORA_AT_TXSIZE "AT+TXSIZE"  // tx size
#define LORA_AT_LINKCHK "AT+LINKCHK"  // link check req

#define LORA_AT_TX "AT+TX"  // tx data, AT+TX port:data in hex
#define LORA_AT_RX "AT+RX"  // rx data, AT+RX port:data in hex

void linkwan_serial_input(uint8_t cmd);
int linkwan_serial_output(uint8_t *buffer, int len);

#endif
