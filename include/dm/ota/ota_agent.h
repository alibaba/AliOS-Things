/*
 * @file ota_agent.h
 *
 * This is an include file of OTA agent transporting with clould.
 *
 * @author    zixun
 * @version   V3.0
 * @date      2019-12-01
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef OTA_AGENT_H
#define OTA_AGENT_H

#if defined OTA_CONFIG_BLE
#include <breeze.h>
#endif

/*******************************************************************
***********                OTA Agent                  **************
***********                   |                       **************
***********                   V                       **************
***********             Service manager               **************
***********  device ----- inform version -----> cloud **************
***********  device ---- subcribe upgrade ----> cloud **************
***********                   |                       **************
***********                   V                       **************
***********             Transport module              **************
***********  device <---- transport message --- cloud **************
***********  device <-- new version:url,sign -- cloud **************
***********                   |                       **************
***********                   V                       **************
***********             Download module               **************
***********  device <---- download firmware --- cloud **************
***********                   |                       **************
***********                   V                       **************
***********             Common hal module             **************
***********  device ---- update image ----> ota part  **************
***********                   |                       **************
***********                   V                       **************
***********             Verify module                 **************
***********  device ---- verfiy firmware ----- verify **************
***********  device ----- report status -----  reboot **************
***********                   |                       **************
***********                   V                       **************
***********                MCU module                 **************
***********  device ----- send FW ------> MCU         **************
********************************************************************/

#define OTA_VERSION "3.1.0"

/**
 *  ENUM: OTA Agent ERRNO.
 */
typedef enum {
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
        OTA_VERIFY_MD5_FAIL       = -10, /*OTA verfiy MD5 failed.*/
        OTA_VERIFY_SHA2_FAIL      = -11, /*OTA verfiy SH256 failed.*/
        OTA_VERIFY_RSA_FAIL       = -12, /*OTA verfiy RSA failed.*/
        OTA_VERIFY_IMAGE_FAIL     = -13, /*OTA verfiy image failed.*/
        OTA_UPGRADE_WRITE_FAIL    = -14, /*OTA upgrade write failed.*/
        OTA_UPGRADE_PARAM_FAIL    = -15, /*OTA upgrade parameter failed.*/
        OTA_UPGRADE_FW_SIZE_FAIL  = -16, /*OTA upgrade FW too big.*/
        OTA_UPGRADE_SET_BOOT_FAIL = -17, /*OTA upgrade set boot failed.*/
        OTA_CUSTOM_CALLBAK_FAIL   = -18, /*OTA custom callback failed.*/
        OTA_MCU_INIT_FAIL         = -19, /*OTA MCU init failed.*/
        OTA_MCU_VERSION_FAIL      = -20, /*OTA MCU version failed.*/
        OTA_MCU_NOT_READY         = -21, /*OTA MCU not ready.*/
        OTA_MCU_REBOOT_FAIL       = -22, /*OTA MCU fail to reboot.*/
        OTA_MCU_HEADER_FAIL       = -23, /*OTA MCU header error.*/
        OTA_MCU_UPGRADE_FAIL      = -24, /*OTA MCU upgrade fail.*/
        OTA_INVALID_PARAMETER     = -25, /*OTA INVALID PARAMETER.*/
} OTA_ERRNO_E;

#define OTA_URL_LEN  256 /*OTA download url max len*/
#define OTA_HASH_LEN 66  /*OTA download file hash len*/
#define OTA_SIGN_LEN 256 /*OTA download file sign len*/
#define OTA_VER_LEN  64  /*OTA version string max len*/

/**
 *  Struct: OTA boot parameter.
 */
typedef struct  {
    unsigned int   dst_adr;    /*Single Bank: Destination Address: APP partition.*/
    unsigned int   src_adr;    /*Single Bank: Copy from Source Address: OTA partition.*/
    unsigned int   len;        /*Single Bank: Download file len */
    unsigned short crc;        /*Single Bank: Download file CRC */
    unsigned short upg_flag;   /*Upgrade flag: OTA_UPGRADE_ALL OTA_UPGRADE_XZ OTA_UPGRADE_DIFF*/
    unsigned char  boot_count; /*Boot count: When >=3 Rollback to old version in BL for dual-banker boot*/
             int   upg_status; /*OTA upgrade status*/
    unsigned char  hash_type;  /*OTA download hash type*/

    char  url[OTA_URL_LEN];    /*OTA download url*/
    char  sign[OTA_SIGN_LEN];  /*OTA download file sign*/
    char  hash[OTA_HASH_LEN];  /*OTA download file hash*/
    char  ver[OTA_VER_LEN];    /*OTA get version*/

    unsigned int   old_size;     /*Diff upgrade: patch old data size*/
    unsigned short patch_num;    /*Diff upgrade: patch num*/
    unsigned short patch_status; /*Diff upgrade: patch status*/
    unsigned int   patch_off;    /*Diff upgrade: patch offset*/
    unsigned int   new_off;      /*Diff upgrade: patch new data offset*/
    unsigned int   new_size;     /*Diff upgrade: patch new data size*/
    unsigned int   upg_magic;    /*OTA upgrade image magic*/
    unsigned char  reserved[14]; /*OTA Reserved*/
    unsigned short param_crc;    /*OTA Parameter crc*/
} ota_boot_param_t;

/**
 *  Struct:  OTA sign info.
 */
typedef struct {
    unsigned int encrypto_magic; /*encrypto type: RSA:0xAABBCCDD or ECC:0xDDCCBBAA*/
    char padding_mode;           /*sign padding mode:PKCS1 or OAEP*/
    char sign_hash_type;         /*sign hash type*/
    char abstract_hash_type;     /*calculate image hash type*/
    char reserved[97];           /*reserved buf*/
    char hash[32];               /*image hash value(sha256)*/
    char signature[256];         /*image digest signature*/
} ota_sign_info_t;

/**
 *  Struct:  OTA image info.
 */
typedef struct
{
    unsigned int   image_magic;   /* image magic */
    unsigned int   image_size;    /* image size */
    unsigned char  image_md5[16]; /* image md5 info */
    unsigned char  image_num;     /* image number */
    unsigned char  image_res;     /* image resouce */
    unsigned short image_crc16;   /* image crc16 */
} ota_image_info_t;

typedef struct
{
    ota_sign_info_t *sign_info;   /* Image sign info */
    ota_image_info_t *image_info; /* Package Image info */
} ota_image_header_t;

/* OTA service manager */
typedef struct ota_service_s {
    char pk[20+1];                /* Product Key */
    char ps[64+1];                /* Product secret */
    char dn[32+1];                /* Device name */
    char ds[64+1];                /* Device secret */
    unsigned char dev_type;       /* device type: 0-->main dev 1-->sub dev */
    unsigned char ota_process;
#if defined OTA_CONFIG_BLE
    void (*on_message)(breeze_otainfo_t *breeze_info);
#endif
    int (*on_upgrade)(struct ota_service_s* ctx, char* ver, char* url); /* new version ready, if OTA upgrade or not by User */
    int (*on_data)(char *buf, int len);                 /* receive data callback of upgrade firmware */
    int (*on_percent)(int per);                         /* report percentage to clould */
    int (*on_boot)(ota_boot_param_t *ota_param);        /* Upgrade complete to reboot to the new version */
    ota_image_header_t header;                          /* OTA Image header */
    ota_boot_param_t *ota_param;                        /* OTA Upgrade parameters */
} ota_service_t;

/* OTA service APIs */
/**
 * ota_service_init  ota service init .
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_service_init(ota_service_t *ctx);

/**
 * ota_service_start  ota service start.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_service_start(ota_service_t *ctx);

/**
 * ota_service_deinit  ota service deinit.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_service_deinit(ota_service_t *ctx);

/**
 * ota_sevice_parse_msg  ota service parse message.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_sevice_parse_msg(ota_service_t *ctx, const char *json);

/**
*
* ota callback
*
**/
enum {
    OTA_CB_ID_UPGRADE = 1,  /* on_upgrade */
    OTA_CB_ID_DATA    = 2,  /* on_data */
    OTA_CB_ID_PERCENT = 3,  /* on_percnet */
    OTA_CB_ID_BOOT    = 4,  /* on_boot */
};

/**
 * ota_service_register_cb  ota service register callback.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_service_register_cb(ota_service_t *ctx, int id, void* cb);

/***************************************************************
*** OTA transport module:transport message with MQTT or CoAP ***
****************************************************************/
/**
 * ota_transport_init  init transport with MQTT or CoAP.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_init(ota_service_t *ctx);

/**
 * ota_transport_inform  OTA inform version to cloud.
 *
 * @param[in] ota_service_t *ctx   ota service context
 * @param[in]          char *ver   version string
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_inform(ota_service_t *ctx, char *ver);

/**
 * ota_transport_upgrade  subscribe OTA upgrade to clould.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_upgrade(ota_service_t *ctx);

/**
 * ota_transport_upgrade  report status to cloud.
 *
 * @param[in] ota_service_t *ctx    ota service context
 * @param[in]            int status ota upgrade status
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_status(ota_service_t *ctx, int status);

/**
 * ota_transport_deinit  destroy ota service context
 *
 * @param[in] ota_service_t *ctx  ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_deinit(ota_service_t *ctx);

/**
 * ota_ble_transport_msg  ota ble transport message.
 *
 * @param[in] breeze_otainfo_t *ble_info  ble ota info.
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
#if defined OTA_CONFIG_BLE
void ota_ble_transport_msg(breeze_otainfo_t *ble_info);
#endif

/***************************************************************
*** OTA download module: download image with HTTP or CoaP    ***
****************************************************************/
/**
 * ota_download_init                ota download init.
 *
 * @param[in] ota_service_t *ctx    ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init fail.
 * @return OTA_DOWNLOAD_HEAD_FAIL  OTA download header fail.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect fail.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request fail.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive fail.
 */
int ota_download_init(ota_service_t *ctx);

/**
 * ota_download_start               ota download start.
 *
 * @param[in] ota_service_t *ctx    ota service context
 * @param[in]          char *url    ota version url
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init fail.
 * @return OTA_DOWNLOAD_HEAD_FAIL  OTA download header fail.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect fail.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request fail.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive fail.
 */
int ota_download_start(ota_service_t *ctx, char *url);

/**
 * ota_download_image_header        ota download image header.
 *
 * @param[in] ota_service_t *ctx    ota service context
 * @param[in]          char *url    ota version url
 * @param[in]  unsigned int size    ota image size
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init fail.
 * @return OTA_DOWNLOAD_HEAD_FAIL  OTA download header fail.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect fail.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request fail.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive fail.
 */
int ota_download_image_header(ota_service_t *ctx, char *url, unsigned int size);

/**
 * ota_download_deinit              ota download deinit.
 *
 * @param[in] ota_service_t *ctx    ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init fail.
 * @return OTA_DOWNLOAD_HEAD_FAIL  OTA download header fail.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect fail.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request fail.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive fail.
 */
int ota_download_deinit(ota_service_t *ctx);

/***************************************************************
*** OTA hal module: update image to OTA partition:ota_hal.h  ***
****************************************************************/
/**
 * ota_read_parameter   ota read parameter from flash.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_read_parameter(ota_boot_param_t *param);

/**
 * ota_update_parameter  ota update parameter to flash.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_update_parameter(ota_boot_param_t *param);

/***************************************************************
***   OTA verify module: hash:md5/sha256 sign:RSA            ***
****************************************************************/
/**
 *  Struct:  MD5 SHA256 Context.
 */
#if defined OTA_CONFIG_ITLS
typedef struct {
    size_t size;
    void *ali_ctx;
} ota_md5_context, ota_sha256_context;
#else
/**
 *  Struct:  MD5 Context.
 */
typedef struct
{
    unsigned int  total[2];
    unsigned int  state[4];
    unsigned char buffer[64];
} ota_md5_context;

/**
 *  Struct:  SHA256 Context.
 */
typedef struct {
    unsigned int  total[2];
    unsigned int  state[8];
    unsigned char buffer[64];
    int is224;
} ota_sha256_context;
#endif

/**
 *  Struct:  ota sign context.
 */
typedef struct {
    char sign_enable;              /* enable sign */
    unsigned char sign_value[256]; /* sign value */
} ota_sign_t;

/**
 *  Struct:  ota hash context.
 */
typedef struct {
    unsigned char hash_method;         /* hash method: md5, sha256 */
    union {
        ota_md5_context md5_ctx;       /* md5 hash context */
        ota_sha256_context sha256_ctx; /* sh256 hash context */
    };
} ota_hash_ctx_t;

/**
 * ota_hash_init  ota hash init.
 *
 * @param[in] ota_hash_ctx_t *ctx   OTA hash context
 * @param[in] unsigned char type    OTA hash type
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_init(ota_hash_ctx_t *ctx, unsigned char type);

/**
 * ota_hash_update  ota hash update.
 *
 * @param[in] ota_hash_ctx_t *ctx      OTA hash context
 * @param[in] const unsigned char *buf OTA hash buf
 * @param[in] unsigned int len         OTA hash len
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_update(ota_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len);

/**
 * ota_hash_final  OTA final hash.
 *
 * @param[in] ota_hash_ctx_t *ctx      OTA hash context
 * @param[in]  unsigned char *buf      OTA hash digest
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_hash_final(ota_hash_ctx_t *ctx, unsigned char *dgst);

/**
 * ota_verify_rsa  OTA verify RSA sign.
 *
 * @param[in]  unsigned char *sign  OTA firmware sign
 * @param[in]     const char *hash  OTA firmware hash
 * @param[in]  unsigned char hash_type  OTA hash type
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_verify_rsa(unsigned char *sign, const char *hash, unsigned char hash_type);

/**
 * ota_check_image  OTA check image.
 *
 * @param[in]  unsigned int size    OTA image size.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_VERIFY_MD5_FAIL      OTA verfiy MD5 fail.
 * @return OTA_VERIFY_SHA2_FAIL     OTA verfiy SH256 fail.
 * @return OTA_VERIFY_RSA_FAIL      OTA verfiy RSA fail.
 * @return OTA_VERIFY_IMAGE_FAIL    OTA verfiy image fail.
 */
int ota_check_image(unsigned int size);
#endif /* OTA_AGNET_H */

