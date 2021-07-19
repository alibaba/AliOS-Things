/** @defgroup ota_agent_api
 * @{
 *
 * This is an include file of OTA agent transporting with clould.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef OTA_AGENT_H
#define OTA_AGENT_H

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

#define OTA_VERSION      "3.3.0"
#define OTA_TRANSTYPE    "1"
#define OTA_OS_TYPE      "0"
#define OTA_BOARD_TYPE   "0"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_ota OTA
 *  OTA upgrade service.
 *
 *  @{
 */

/* OTA upgrade flag */
#define OTA_UPGRADE_CUST   0x8778 /* upgrade user customize image */
#define OTA_UPGRADE_ALL    0x9669 /* upgrade all image: kernel+framework+app */
#define OTA_UPGRADE_XZ     0xA55A /* upgrade xz compressed image */
#define OTA_UPGRADE_DIFF   0xB44B /* upgrade diff compressed image */
#define OTA_UPGRADE_KERNEL 0xC33C /* upgrade kernel image only */
#define OTA_UPGRADE_APP    0xD22D /* upgrade app image only */
#define OTA_UPGRADE_FS     0x7083 /* upgrade fs image only */
#define OTA_BIN_MAGIC_APP     0xabababab
#define OTA_BIN_MAGIC_KERNEL  0xcdcdcdcd
#define OTA_BIN_MAGIC_ALL     0xefefefef
#define OTA_BIN_MAGIC_MCU     0xefcdefcd
#define OTA_BIN_MAGIC_FS      0xabcdabcd

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

typedef enum {
    OTA_REPORT_UPGRADE_ERR  = -1, /* ota report upgrading failed to cloud*/
    OTA_REPORT_DOWNLOAD_ERR = -2, /* ota report downloading failed to cloud*/
    OTA_REPORT_VERIFY_ERR   = -3, /* ota report image verified err to cloud*/
    OTA_REPORT_BURN_ERR     = -4, /* ota report image burning failed to cloud*/
} OTA_REPORT_CLOUD_ERRNO_E;

#define OTA_URL_LEN  256 /*OTA download url max len*/
#define OTA_HASH_LEN 66  /*OTA download file hash len*/
#define OTA_SIGN_LEN 256 /*OTA download file sign len*/
#define OTA_VER_LEN  64  /*OTA version string max len*/

typedef enum {
  OTA_EVENT_UPGRADE_TRIGGER,
  OTA_EVENT_DOWNLOAD,
  OTA_EVENT_INSTALL,
  OTA_EVENT_LOAD,
  OTA_EVENT_REPORT_VER,
} OTA_EVENT_ID;
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
    unsigned char  boot_type;    /*OS boot type:Single boot(0x00), dual boot(0x01)*/
    unsigned char  reserved[13]; /*OTA Reserved*/
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
typedef struct {
    unsigned int   image_magic;   /* image magic */
    unsigned int   image_size;    /* image size */
    unsigned char  image_md5[16]; /* image md5 info */
    unsigned char  image_num;     /* image number */
    unsigned char  image_res;     /* image resouce */
    unsigned short image_crc16;   /* image crc16 */
} ota_image_info_t;

typedef struct {
    ota_sign_info_t *sign_info;   /* Image sign info */
    ota_image_info_t *image_info; /* Package Image info */
} ota_image_header_t;

typedef struct {
    void (*on_user_event_cb)(int event, int errnumb, void *param);
    void *param; /* users paramter */
} ota_feedback_msg_func_t;

typedef int (*report_func)(void *, uint32_t);

typedef int (*triggered_func)(void *, char *, char *, void *);

typedef struct {
    report_func report_status_cb;
    void *param; /* users paramter */
} ota_report_status_func_t;

typedef struct {
    triggered_func triggered_ota_cb;
    void *param; /* users paramter */
} ota_triggered_func_t;

typedef struct {
    char module_name[33];
    char store_path[77];
    int module_type;
} ota_store_module_info_t;

/* OTA service manager */
typedef struct ota_service_s {
    char pk[20+1];                /* Product Key */
    char ps[64+1];                /* Product secret */
    char dn[32+1];                /* Device name */
    char ds[64+1];                /* Device secret */
    unsigned char dev_type;       /* device type: 0-->main dev 1-->sub dev*/
    char module_name[33];         /* module name*/
    unsigned char ota_process;
    int module_numb;
    ota_store_module_info_t *module_queue_ptr;
    ota_feedback_msg_func_t feedback_func;
    ota_report_status_func_t report_func;               /* report percentage to clould */
    ota_triggered_func_t ota_triggered_func;            /* new version ready, if OTA upgrade or not by User */
    int (*on_boot)(ota_boot_param_t *ota_param);        /* Upgrade complete to reboot to the new version */
    ota_image_header_t header;                          /* OTA Image header */
    void *mqtt_client;                                  /* mqtt client */                      /* OTA Upgrade parameters */
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
 * ota_service_start                ota service start and file store in flash
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
 * ota_download_to_fs_service  ota service submodule start.
 *
 * @param[in] void *ctx            ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
 int ota_download_to_fs_service(void *ota_ctx , char *file_path);

/**
 * ota_install_jsapp  ota service submodule start.
 *
 * @param[in] void *ota_ctx        ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_install_jsapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path);

/**
 * ota_report_module_version      ota report module version.
 *
 * @param[in]         void *ctx   ota service context
 * @param[in] char *module_name   want tp report module name
 * @param[in]     char *version   module file version
 *
 * @return OTA_SUCCESS             OTA success.
 * @return -1                      OTA transport init fail.
 */
int ota_report_module_version(void *ota_ctx, char *module_name, char *version);

/**
 * ota_service_param_reset;
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return NULL
 */
void ota_service_param_reset(ota_service_t *ctx);

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
 * ota_register_module_store ota register store moudle information buf to ctx;
 *
 *
 * @param[in] ota_service_t             *ctx ota service context
 * @param[in] ota_store_module_info_t *queue store moudle information buf ptr
 * @param[in] int                  queue_len module buf size
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  Get information failed.
 */
int ota_register_module_store(ota_service_t *ctx, ota_store_module_info_t *queue, int queue_len);
/**
 * ota_set_module_information      ota set module information to DB, include:
 *                                 module name, store path, module type.
 *
 * @param[in] ota_service_t *ctx ota service context
 * @param[in] char  *module_name ota module name
 * @param[in] char   *store_path want to store module file path
 * @param[in] int    module_type upgrade type: OTA_UPGRADE_ALL.etc.
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  Get information failed.
 */
int ota_set_module_information(ota_service_t *ctx, char *module_name,
                               char *store_path, int module_type);
/**
 * ota_get_module_information      ota get module information,include:
 *                                 module name, store path, module type.
 *
 * @param[in] ota_service_t *ctx   ota service context
 * @param[in]  char *module_name   ota module name
 * @param[in] ota_store_module_info_t *module_info want to store module information var
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  Get information failed.
 */
int ota_get_module_information(ota_service_t *ctx, char *module_name, ota_store_module_info_t *module_info);
/**
 * ota_register_boot_cb            ota register boot callback.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_register_boot_cb(ota_service_t *ctx, void *cb, void *param);
/**
 * ota_register_trigger_msg_cb     ota register trigger ota callback.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_register_trigger_msg_cb(ota_service_t *ctx, void *cb, void *param);
/**
 * ota_register_report_percent_cb  ota register file download process callback.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_register_report_percent_cb(ota_service_t *ctx, void *cb, void *param);
/**
 * ota_register_feedback_msg_cb  ota service register callback.
 *
 * @param[in] ota_service_t *ctx   ota service context
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_register_feedback_msg_cb(ota_service_t *ctx, void *cb, void *param);
/***************************************************************
*** OTA transport module:transport message with MQTT or CoAP ***
****************************************************************/
/**
 * ota_transport_inform  OTA inform version to cloud.
 *
 * @param[in]   void *mqttclient   mqtt client ptr
 * @param[in]           char *pk   product key value
 * @param[in]           char *dn   device name
 * @param[in]  char *module_name   want to report module name, when module_name == NULL, report default module ver
 * @param[in]          char *ver   version string
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_inform(void *mqttclient, char *pk, char *dn, char *module_name, char *ver);

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
 * @param[in] void *param  ota service context
 * @param[in] int   status ota upgrade status
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_TRANSPORT_INT_FAIL  OTA transport init fail.
 * @return OTA_TRANSPORT_PAR_FAIL  OTA transport parse fail.
 * @return OTA_TRANSPORT_VER_FAIL  OTA transport verion is too old.
 */
int ota_transport_status(void *param, int status);

/***************************************************************
*** OTA download module: download image with HTTP or CoaP    ***
****************************************************************/
/**
 * ota_download_start    OTA download start
 *
 * @param[in]              char *url  download url
 * @param[in]   unsigned int url_len  download url length
 * @param[in] report_func repot_func  report http downloading status function
 * @param[in]       void *user_param  user's param for repot_func
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_start(char *url, unsigned int url_len, report_func repot_func, void *user_param);

/**
 * ota_download_store_fs_start    OTA download file start and store in fs
 *
 * @param[in]              char *url  download url
 * @param[in]   unsigned int url_len  download url length
 * @param[in]       char *store_path  store file path and name eg:/root/test.bin
 * @param[in] report_func report_func report http downloading status function
 * @param[in]       void *user_param  user's param for repot_func
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init failed.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect failed.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request failed.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive failed.
 */
int ota_download_store_fs_start(char *url, unsigned int url_len, char *store_path,
                                report_func report_func, void *user_param);
/**
 * ota_download_image_header        ota download image header.
 *
 * @param[in]   ota_service_t *ctx    ota service context
 * @param[in]            char *url    download url
 * @param[in] unsigned int url_len    download url length
 * @param[in]    unsigned int size    ota image size
 *
 * @return OTA_SUCCESS             OTA success.
 * @return OTA_DOWNLOAD_INIT_FAIL  OTA download init fail.
 * @return OTA_DOWNLOAD_HEAD_FAIL  OTA download header fail.
 * @return OTA_DOWNLOAD_CON_FAIL   OTA download connect fail.
 * @return OTA_DOWNLOAD_REQ_FAIL   OTA download request fail.
 * @return OTA_DOWNLOAD_RECV_FAIL  OTA download receive fail.
 */
int ota_download_image_header(ota_service_t *ctx, char *url, unsigned int url_len, unsigned int size);

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

/**
 * ota_get_fs_version  ota get fs image version.
 *
 * @param[in] char *ver_buf         store version buffer
 * @param[in] cint ver_buf_len      store version buffer len
 *
 * @return 0                        get version success.
 * @return -1                       get version fail.
 */
int ota_get_fs_version(char *ver_buf, int ver_buf_len);

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

/**
 * ota_jsapp_version_get            OTA parase js script version
 *
 * @param[in]  char *version        store version buf.
 * @param[in]  char *file_path      js.app json file store path.
 *
 * @return 0                        get version success.
 * @return -1                       get version failed.
 */
int ota_jsapp_version_get(char *version, char *file_path);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /* OTA_AGNET_H */