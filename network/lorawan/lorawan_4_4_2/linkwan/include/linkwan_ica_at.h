/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_AT_H
#define LINKWAN_AT_H

#define AT_CMD "AT"

#define AT_ERROR "+CME ERROR:"

// mandatory
#define LORA_AT_CJOINMODE "+CJOINMODE"  // join mode
#define LORA_AT_CFREQBAND "+CFREQBAND"  // freq band (OTAA)
#define LORA_AT_CDEVEUI "+CDEVEUI"  // dev eui (OTAA)
#define LORA_AT_CAPPEUI "+CAPPEUI"  // app eui (OTAA)
#define LORA_AT_CAPPKEY "+CAPPKEY"  // app key (OTAA)
#define LORA_AT_CDEVADDR "+CDEVADDR"  // dev addr (ABP)
#define LORA_AT_CAPPSKEY "+CAPPSKEY"  // sapp key (ABP)
#define LORA_AT_CNWKSKEY "+CNWKSKEY"  // nwk skey (ABP)
#define LORA_AT_CADDMULTICAST "+CADDMULTICAST"  // add mcast
#define LORA_AT_CDELMULTICAST "+CDELMULTICAST"  // del mcast
#define LORA_AT_CNUMMULTICAST "+CNUMMULTICAST"  // mcast num
#define LORA_AT_CFREQBANDMASK "+CFREQBANDMASK"  // freqband mask
#define LORA_AT_CULDLMODE "+CULDLMODE"  // ul and dl

#define LORA_AT_CWORKMODE "+CWORKMODE"  // work mode
#define LORA_AT_CREPEATERFREQ "+CREPEATERFREQ"  // repeater freq
#define LORA_AT_CCLASS "+CCLASS"  // class
#define LORA_AT_CBL "+CBL"  // battery level
#define LORA_AT_CSTATUS "+CSTATUS"  // cstatus
#define LORA_AT_CJOIN "+CJOIN"  // OTTA join

#define LORA_AT_DTRX "+DTRX"  // tx
#define LORA_AT_DRX "+DRX"  // rx

#define LORA_AT_CCONFIRM "+CCONFIRM"  //cfm or uncfm
#define LORA_AT_CAPPPORT "+CAPPPORT"  // app port
#define LORA_AT_CDATARATE "+CDATARATE"  // data rate
#define LORA_AT_CRSSI "+CRSSI"  // rssi
#define LORA_AT_CNBTRIALS "+CNBTRIALS"  // nb trans
#define LORA_AT_CRM "+CRM"  // report mode
#define LORA_AT_CTXP "+CTXP"  // tx power
#define LORA_AT_CLINKCHECK "+CLINKCHECK"  // link check
#define LORA_AT_CADR "+CADR"  // ADR
#define LORA_AT_CRXP "+CRXP"  // rx win params
#define LORA_AT_CFREQLIST "+CFREQLIST"  // freq list
#define LORA_AT_CRX1DELAY "+CRX1DELAY"  // rx1 win delay
#define LORA_AT_CSAVE "+CSAVE"  // save cfg
#define LORA_AT_CRESTORE "+CRESTORE"  // restore def cfg


// repeater
#define LORA_AT_CREPEATERFILTER "+CREPEATERFILTER"

// optional
#define LORA_AT_CGMI "+CGMI"  // manufacture identification
#define LORA_AT_CGMM "+CGMM"  // model identification
#define LORA_AT_CGMR "+CGMR"  // revision info
#define LORA_AT_CGSN "+CGSN"  // product serial number id
#define LORA_AT_CGBR "+CGBR"  // baud rate on UART interface

#define LORA_AT_ILOGLVL "+ILOGLVL"  // log level
#define LORA_AT_IREBOOT "+IREBOOT"
#define LORA_AT_IDEFAULT "+IDEFAULT"

#define LORA_AT_CTEST "+CTEST"

void linkwan_serial_input(uint8_t cmd);
int linkwan_serial_output(uint8_t *buffer, int len);

#endif
