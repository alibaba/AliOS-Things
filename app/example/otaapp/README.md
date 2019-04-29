## Contents

```sh
OTA
├── hal
├── ota_service.c
├── src
│   ├── 2nd_boot
│   ├── device
│   ├── download
│   ├── transport
│   └── verify
└── aos.mk
```

## Introduction

An over-the-air update is the wireless delivery of new software or data to smart devices, especially IoT devices. Wireless carriers and OEMs typically use over-the-air (OTA) updates to deploy the new operating systems and the software app to these devices.

## Features

1. Differential incremental upgrade;
2. Dual banker:AB partition upgrade to support rollback to old version;
3. Secure download channel;
4. Firmware digital signature verification.

## Dependencies

Linkkit MQTT channel
Linkkit CoAP channel

### Supported Boards

- mk3060
- mk3080
- developerkit
- esp8266

## API

User service APIs:

```c
typedef struct {
    unsigned char  inited;  /*If is inted*/
    char  pk[20+1];/*Product Key*/
    char  ps[64+1];/*Product secret*/
    char  dn[32+1];/*Device name*/
    char  ds[64+1];/*Device secret*/
    OTA_PROTCOL_E  trans_protcol;  /*default:0--> MQTT 1-->COAP*/
    OTA_PROTCOL_E  dl_protcol;     /*default:3--> HTTPS 1-->COAP 2-->HTTP*/
    unsigned char  sign_type;      /*default:0--> sha256 1--> md5 2-->RSA*/
    char           ota_ver[OTA_MAX_VER_LEN];  /*OTA FW version*/
    char           sys_ver[OTA_MAX_VER_LEN];  /*OTA System version*/
    ota_transport_t *h_tr;    /*OTA tansport manager*/
    ota_download_t  *h_dl;    /*OTA download manager*/
    void*    h_ch;            /*OTA channel handle:mqtt,coap*/
} ota_service_t;

/*OTA export service APIs*/
int ota_service_init(ota_service_t* ctx);
int ota_service_deinit(ota_service_t* ctx);
```

for sample code please check [otaapp](../../../app/example/otaapp/).
## RTOS build

```sh
# generate otaapp@esp8266 default config
aos make otaapp@esp8266 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

## run CLI CMDs
1. connect network
```
netmgr connect ssid passwd
```
2. run ota demo

```
OTA_APP pk dn ds ps
```

## Reference

* https://github.com/alibaba/AliOS-Things/wiki/OTA-Tutorial
* https://github.com/alibaba/AliOS-Things/wiki/OTA-Flash-Partitions-Overview
* https://mp.weixin.qq.com/s/Pb8Lleuww1r7qQJHu5ON8g
