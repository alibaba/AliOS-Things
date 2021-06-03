#ifndef __ALIPAY_XP_SDK_API_H__
#define __ALIPAY_XP_SDK_API_H__

#include "alipay_xp_sdk_ota.h"
//export to app/isv inc file


/****SDK默认支持***/

//应用代码注册fota接口
int    alipay_xp_sdk_fota_api_attach(xp_ota_api_t api);

//去云端主动check设备所有的应用ota信息(fota+应用)
int    alipay_xp_sdk_ota_default_check(void);


#endif

