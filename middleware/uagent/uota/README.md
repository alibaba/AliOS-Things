Contents
OTA
├── Config.in
├── hal
│   ├── hal.mk
│   ├── ota_hal_host.c
│   ├── ota_hal_module.c
│   ├── ota_hal_os.c
│   ├── ota_hal_os.h
│   ├── ota_hal_plat.c
│   └── ota_hal_plat.h
├── inc
│   ├── ota_log.h
│   └── ota_service.h
├── LICENSE
├── Makefile
├── ota_service.c
├── README.md
├── src
│   ├── 2nd_boot
│   │   ├── 2nd_boot.mk
│   │   ├── nbpatch
│   │   ├── rec_2boot.c
│   │   ├── rec_common.c
│   │   ├── rec_common.h
│   │   ├── rec_define.h
│   │   ├── rec_libc.c
│   │   ├── rec_main.c
│   │   ├── rec_pub.h
│   │   └── xz
│   ├── device
│   │   └── ble
│   ├── download
│   │   ├── download.mk
│   │   ├── ota_download_coap.c
│   │   └── ota_download_http.c
│   ├── transport
│   │   ├── ota_transport_coap.c
│   │   ├── ota_transport_mqtt.c
│   │   └── transport.mk
│   └── verify
│       ├── base64
│       ├── crc
│       ├── hash
│       ├── ota_public_key_config.h
│       ├── ota_rsa_param.h
│       ├── ota_rsa_verify.c
│       ├── ota_rsa_verify.h
│       ├── ota_verify.c
│       ├── ota_verify.h
│       ├── rsa
│       └── verify.mk
└── uota.mk

Introduction
An over-the-air update is the wireless delivery of new software or data to smart devices, especially IoT devices. Wireless carriers and OEMs typically use over-the-air (OTA) updates to deploy the new operating systems and the software app to these devices.

Features
1. Differential incremental upgrade;
2. Dual banker:AB partition upgrade to support rollback to old version;
3. Secure download channel;
4. Firmware digital signature verification.

Dependencies
Linkkit MQTT channel
Linkkit CoAP channel

API
User service APIs:
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

Usage:
## RTOS build
cd ROOT DIR;
aos make otaapp@board;

## run CLI CMDs
## 1. connect network
netmgr connect ssid passwd
## 2. run ota demo
OTA_APP pk dn ds ps
