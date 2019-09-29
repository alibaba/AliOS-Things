## Contents

```sh
otaapp demo
├── aos.mk
├── Config.in
├── k_app_config.h
├── otaapp.c
├── README.md

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
/*******************************************************************
***********             OTA sequence                  **************
***********                   |                       **************
***********                   V                       **************
***********             OTA service manager           **************
***********  device ----- inform version -----> cloud **************
***********  device ---- subcribe upgrade ----> cloud **************
***********                   |                       **************
***********                   V                       **************
***********             OTA transport module          **************
***********  device <---- transport message --- cloud **************
***********  device <---- get download url ---- cloud **************
***********                   |                       **************
***********                   V                       **************
***********             OTA download module           **************
***********  device <---- download firmware --- cloud **************
***********  device <---- verfiy firmware ----- cloud **************
***********  device ----- report status ----->  cloud **************
***********                   |                       **************
***********                   V                       **************
***********             OTA common hal module         **************
***********  device ----- write image ------- reboot  **************
********************************************************************/

#define OTA_VERSION             "2.0.0"

/* OTA status code */
typedef enum {
        OTA_BREAKPOINT            = 5,   /*OTA breakpoint status*/
        OTA_FINISH                = 4,   /*OTA finish status*/
        OTA_DOWNLOAD              = 3,   /*OTA download status*/
        OTA_TRANSPORT             = 2,   /*OTA transport status*/
        OTA_INIT                  = 1,   /*OTA init status*/
        OTA_SUCCESS               = 0,
        OTA_INIT_FAIL             = -1,  /*OTA init failed.*/
        OTA_TRANSPORT_INT_FAIL    = -2,  /*OTA transport init failed.*/
        OTA_TRANSPORT_PAR_FAIL    = -3,  /*OTA transport parse failed.*/
        OTA_TRANSPORT_VER_FAIL    = -4,  /*OTA transport verion is too old.*/
        OTA_DOWNLOAD_INIT_FAIL    = -5,  /*OTA download init failed.*/
        OTA_DOWNLOAD_HEAD_FAIL    = -6,  /*OTA download header failed.*/
        OTA_DOWNLOAD_CON_FAIL     = -7,  /*OTA download connect failed.*/
        OTA_DOWNLOAD_REQ_FAIL     = -8,  /*OTA download request failed.*/
        OTA_DOWNLOAD_RECV_FAIL    = -9,  /*OTA download receive failed.*/
        OTA_VERIFY_MD5_FAIL       = -10,  /*OTA verfiy MD5 failed.*/
        OTA_VERIFY_SHA2_FAIL      = -11,  /*OTA verfiy SH256 failed.*/
        OTA_VERIFY_RSA_FAIL       = -12, /*OTA verfiy RSA failed.*/
        OTA_VERIFY_IMAGE_FAIL     = -13, /*OTA verfiy image failed.*/
        OTA_UPGRADE_WRITE_FAIL    = -14, /*OTA upgrade write failed.*/
        OTA_UPGRADE_PARAM_FAIL    = -15, /*OTA upgrade parameter failed.*/
        OTA_UPGRADE_FW_SIZE_FAIL  = -16, /*OTA upgrade FW too big.*/
        OTA_UPGRADE_SET_BOOT_FAIL = -17, /*OTA upgrade set boot failed.*/
} OTA_STATUS_E;

/* OTA transport module: transport message with MQTT,CoAP */
int ota_transport_init(void);                               /* OTA transport init */
int ota_transport_inform(char* pk, char*dn, char* ver);     /* OTA transport inform version to cloud*/
int ota_transport_upgrade(char* pk, char*dn);               /* OTA transport subcribe upgrade from cloud */
int ota_transport_status(char* pk, char*dn, int status);    /* OTA transport report status:[1-100] percent [<0] errno */
int ota_transport_deinit(void);                             /* OTA transport deinit */

/* OTA download module: download FW with HTTPs, CoAP */
int ota_download_init(void);            /* OTA download init */
int ota_download_start(char* url);      /* OTA download start */
int ota_download_stop(void);            /* OTA download stop */
int ota_download_deinit(void);          /* OTA download deinit */

/* OTA upgrade magic <--> upg_flag */
#define OTA_UPGRADE_CUST       0x8778       /* upgrade user customize image */
#define OTA_UPGRADE_ALL        0x9669       /* upgrade all image: kernel+framework+app */
#define OTA_UPGRADE_XZ         0xA55A       /* upgrade xz compressed image */
#define OTA_UPGRADE_DIFF       0xB44B       /* upgrade diff compressed image */
#define OTA_UPGRADE_KERNEL     0xC33C       /* upgrade kernel image only */
#define OTA_UPGRADE_APP        0xD22D       /* upgrade app image only */

#define OTA_URL_LEN                  256   /*OTA download url max len*/
#define OTA_HASH_LEN                 66    /*OTA download file hash len*/
#define OTA_SIGN_LEN                 256   /*OTA download file sign len*/
#define OTA_VER_LEN                  64    /*OTA version string max len*/
#define OTA_MSG_LEN                  256   /*OTA topic message max len*/

typedef struct  {
    unsigned int   dst_adr;     /*Single Bank: Destination Address: APP partition.*/
    unsigned int   src_adr;     /*Single Bank: Copy from Source Address: OTA partition.*/
    unsigned int   len;         /*Single Bank: Download file len */
    unsigned short crc;         /*Single Bank: Download file CRC */
    unsigned short upg_flag;    /*Upgrade flag: OTA_UPGRADE_ALL OTA_UPGRADE_XZ OTA_UPGRADE_DIFF*/
    unsigned char  boot_count;  /*Boot count: When >=3 Rollback to old version in BL for dual-banker boot*/
             int   upg_status;  /*OTA upgrade status*/
    unsigned char  hash_type;   /*OTA download hash type*/
    char  url[OTA_URL_LEN];     /*OTA download url*/
    char  sign[OTA_SIGN_LEN];   /*OTA download file sign*/
    char  hash[OTA_HASH_LEN];   /*OTA download file hash*/
    char  ver[OTA_VER_LEN];     /*OTA get version*/
    unsigned int   old_size;    /*Diff upgrade: patch old data size*/
    unsigned short patch_num;   /*Diff upgrade: patch num*/
    unsigned short patch_status;/*Diff upgrade: patch status*/
    unsigned int   patch_off;   /*Diff upgrade: patch offset*/
    unsigned int   new_off;     /*Diff upgrade: patch new data offset*/
    unsigned int   new_size;    /*Diff upgrade: patch new data size*/
    unsigned char  reserve[18];
    unsigned short param_crc;   /*OTA Parameter crc*/
} __attribute__((aligned(16))) ota_boot_param_t;

/* OTA common hal module */
int ota_hal_init(ota_boot_param_t *parm);                                        /* init OTA partition and check */
int ota_hal_write(unsigned int *off_set, char *in_buf, unsigned int in_buf_len); /* Write data to flash */
int ota_hal_read(unsigned int *off_set, char *out_buf, unsigned int out_buf_len);/* read data from flash */
int ota_hal_boot(ota_boot_param_t *parm);                                        /* update parameter and reboot */
int ota_hal_rollback(void);                                                      /* clear rollock flag */
const char *ota_hal_version(unsigned char dev_type, char *dn);                   /* get current system version */
int ota_read_parameter(ota_boot_param_t *ota_param);                             /* ota read parameter */
int ota_update_parameter(ota_boot_param_t *ota_param);                           /* ota update parameter */
/* OTA service manager */
typedef struct {
    unsigned char inited;               /* If is inted */
    char pk[20+1];                      /* Product Key */
    char ps[64+1];                      /* Product secret */
    char dn[32+1];                      /* Device name */
    char ds[64+1];                      /* Device secret */
    unsigned char dev_type;             /* device type: 0-->main dev 1-->sub dev */
    int (*on_data)(char *buf, int len); /* receive data callback of upgrade firmware */
    int (*on_percent)(int per);         /* report percentage to clould */
    int (*on_boot)(ota_boot_param_t *ota_param);/* Upgrade complete to reboot to the new version */
} ota_service_t;

int ota_service_init(ota_service_t *ctx);   /* Init ota serivice */
int ota_service_deinit(ota_service_t *ctx); /* Deinit ota serivice */
int ota_service_start(ota_service_t *ctx);  /* Start secure download mode */
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
