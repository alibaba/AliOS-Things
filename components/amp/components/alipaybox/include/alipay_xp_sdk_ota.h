#ifndef __ALIPAY_XP_SDK_OTA_H__
#define __ALIPAY_XP_SDK_OTA_H__

//export inc file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define XP_SDK_OTA_STATUE_RECEIVED        (1)
#define XP_SDK_OTA_STATUE_DOWNLOADED      (2)
#define XP_SDK_OTA_STATUE_VERIFIED        (3)
#define XP_SDK_OTA_STATUE_INSTALLED       (4)
#define XP_SDK_OTA_STATUE_INVALID_PARAMS  (-1)
#define XP_SDK_OTA_STATUE_DOWNLOAD_FAIL   (-2)
#define XP_SDK_OTA_STATUE_VERIFIED_FAIL   (-3)
#define XP_SDK_OTA_STATUE_WRITE_FAIL      (-4)
#define XP_SDK_OTA_STATUE_CHECK_FAIL      (-5)
#define XP_SDK_OTA_STATUE_UPGRADE_FAIL    (-6)

//说明:这里down_size是ota下载的加密后文件大小，不是上传的ota平台的固件大小
typedef int (*ota_start_api)(int down_size);
typedef int (*ota_write_api)(int addr,char *data, int len);
typedef int (*ota_read_api) (int addr, char *data, int len);
typedef int (*ota_block_erase_api)(int addr,int block_size);
//说明:这里的ota_size大小是用于上传到ota平台的用于升级的固件大小
typedef int (*ota_upgrade_api)(int ota_size);
typedef int (*ota_block_size_get_api)(void);
typedef int (*ota_max_size_get_api)(void);
typedef int (*ota_seek_api)(int addr,int size);
typedef int (*ota_end_api)(void);
typedef int (*ota_security_get_api)(void);
typedef int (*ota_silent_reboot_api)(void);


//上层注册ota接口
typedef struct{
    //start:以下为必须设置  ----->
  
    //ota 开始接口，用于通知上层有ota任务开始下载
    ota_start_api             start;
    //ota 写接口，用于注册写ota固件的接口，文件/内存/flash          都必须注册
    ota_write_api              w;

    //ota 下载检验完成接口，固件下载解密校验完成之后，sdk内部调用上层注册的此接口，可将升级接口注册进来
    ota_upgrade_api             upgrade;

    //ota 获取最大固件长度接口，用于对于误上传的过大固件的下载拦截,必须注册 
    ota_max_size_get_api        get_max_size;
    
    //ota擦除接口，文件/内存/flash都必须注册，其中addr=0时，是ota开始下载开始的标志
    ota_block_erase_api          erase_block;

    //end:<------以上为必须设置

    //ota 读接口，可不注册，如果注册的话，每次写入sdk内部都会读出来和写入的数据做check
    ota_read_api               r;

    //ota结束回调接口，可不注册，注册的话，上层应用可以通过此接口拿到ota结束进行相关操作
    ota_end_api                  end;

    ota_silent_reboot_api        silent_reboot;
    
    //使用flash裸存固件时候，必须注册，其他形式建议不要注册，注册的话，erase里面要做判断
    ota_block_size_get_api      get_bock_size;

    //ota seek接口，内存和flash可不注册或者注册空实现
    //文件存储固件时候，建议必须注册,可直接写size长度的0xff等dummy byte
    ota_seek_api                seek;

    //定制接口，                     通常情况下不需要外部来注册
    ota_security_get_api        get_security;
}xp_ota_api_t;

#endif


