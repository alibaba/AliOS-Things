/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define UOTA_VERSION   "0.0.1"

#ifndef OTA_SERVICE_H_
#define OTA_SERVICE_H_
#define PRODUCT_KEY_MAXLEN          (20 + 1)
#define PRODUCT_SECRET_MAXLEN       (64 + 1)
#define DEVICE_NAME_MAXLEN          (32 + 1)
#define DEVICE_SECRET_MAXLEN        (64 + 1)

typedef enum{
   OTA_PROTCOL_MQTT=0,
   OTA_PROTCOL_COAP,
   OTA_PROTCOL_HTTP,
   OTA_PROTCOL_HTTPS,
}ota_protcol;

typedef struct {
    int   inited;  /*If is inted*/
    char  pk[PRODUCT_KEY_MAXLEN + 1];/*Product Key*/
    char  ps[PRODUCT_SECRET_MAXLEN + 1];/*Product secret*/
    char  dn[DEVICE_NAME_MAXLEN + 1];/*Device name*/
    char  ds[DEVICE_SECRET_MAXLEN + 1];/*Device secret*/
    char  uuid[64];
    int   trans_protcol;  /*default:0--> MQTT 1-->COAP*/
    int   dl_protcol;    /*default:3--> HTTPS 1-->COAP 2-->HTTP*/
    int   sign_type;     /*default:0--> sha256 1--> md5 2-->RSA*/
    int   firm_size;
    char* dl_url;         /*Dowdload URL*/
    void* h_coap;
} ota_service_manager;

int ota_service_init(ota_service_manager* context);

const void* get_ota_service_manager(void);

const void* ota_get_transport(int protcol);
#endif /* OTA_SERVICE_H_ */
