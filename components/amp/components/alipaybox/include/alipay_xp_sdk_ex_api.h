#ifndef __ALIPAY_XP_SDK_EX_API_H__
#define __ALIPAY_XP_SDK_EX_API_H__

#include "alipay_xp_sdk_ota.h"
#include "alipay_xp_sdk_api.h"

//export to app/isv inc file


#define  XP_DEFAULT_APPID_VERSION  "1.2.0"  //NOT USE in sdk inside

typedef    ant_char *( *xp_dn_dynamic_get_api)(void); 

typedef struct{
    char         * appid;
    char         * version;
}alipay_xp_sdk_ota_info_t;

typedef struct{
    alipay_xp_sdk_ota_info_t * infos;
    int  size;
}alipay_xp_sdk_ota_config_t;

typedef struct{
    char * name;
}alipay_xp_method_desc_t;

typedef struct{
   alipay_xp_method_desc_t  *method;
   int size;
}alipay_xp_method_register_t;

typedef struct{
   char *                        product_key;
   char *                        product_secret;
   char *                        device_secret;
   char *                        device_name;
   char *                        version;
   xp_ota_api_t                  fota_api;
   xp_dn_dynamic_get_api         dn_api;
   alipay_xp_sdk_ota_config_t    config;
}alipay_xp_sdk_device_config_t;

//可由应用代码自行初始化xp接口，目前sdk配置的是sdk内部自动初始化
int    alipay_xp_sdk_device_init(alipay_xp_sdk_device_config_t device);

//应用代码订阅额外的method
int    alipay_xp_sdk_methods_subscribe_add(char ** methods,int size);

//主动向云端发送消息
int    alipay_xp_sdk_push_msg(char * method,char *body);

/****以下代码接口支持需要联系蚂蚁提供配置支持***/

//应用代码注册appid产物的ota接口，xp初始化的时候，产物必须为空
int    alipay_xp_sdk_ota_api_attach(char * appid,xp_ota_api_t api);

//去云端主动check产物appid是否需要升级
int    alipay_xp_sdk_app_ota_check(char  * appid,char * version);

//去云端主动check产物appids是否需要升级
int    alipay_xp_sdk_apps_ota_check(alipay_xp_sdk_ota_info_t * apps,int size);


#endif

