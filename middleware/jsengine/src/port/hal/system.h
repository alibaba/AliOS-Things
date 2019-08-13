/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*文件功能说明:
  本文件定义了系统接口抽象层System Abstract Layer,
  用于统一不同的硬件软件平台的系统接口,如网络相关,kv,文件系统
  不同的平台需要移植
  */

#ifndef BE_SYSTEM_H
#define BE_SYSTEM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <fcntl.h>

#include "aos/vfs.h"
#include "aos/kv.h"
#include "netmgr.h"
#include "network/network.h"

#define jse_malloc aos_malloc
#define jse_calloc aos_calloc
#define jse_realloc aos_realloc
#define jse_free aos_free

#define BE_FS_ROOT_DIR "/spiffs/"

#define be_dirent_t aos_dirent_t
#define be_dir_t aos_dir_t

#define be_open aos_open
#define be_close aos_close
#define be_lseek aos_lseek
#define be_read aos_read
#define be_write aos_write
#define be_sync aos_sync
#define be_stat aos_stat
#define be_opendir aos_opendir
#define be_readdir aos_readdir
#define be_closedir aos_closedir
#define be_unlink aos_unlink
#define be_mkdir aos_mkdir
#define be_rmdir aos_rmdir

#define JSE_MAX_SSID_SIZE MAX_SSID_SIZE
#define JSE_MAX_PWD_SIZE MAX_PWD_SIZE

/***************************************************
 * @fn			hal_system_get_platform_type
 *
 * @brief		获取当前平台类型
 *
 * @param		无
 *
 * @return		字符串.
 **************************************************/
const char *hal_system_get_platform_type();

/**************************************************
 * @fn 		 hal_system_get_ip
 *
 * @brief     获取系统IP
 *
 * @param     ip：存储ip字符串的buffer
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_get_ip(char *ip);

/**************************************************
* @fn 		 hal_system_kv_init
*
* @brief     初始化key-value系统，如ESP使用nvs实现，
             需要先初始化nvs
*
* @param    无
*
* @return 	 成功返回０　失败返回其他
**************************************************/
int32_t hal_system_kv_init();

/**************************************************
 * @fn 		 hal_system_kv_get
 *
 * @brief     key-value获取函数
 *
 * @param     key: key-value的key指针
 *            buffer: 存储value的buffer
 *            buffer_len: buffer的len指针
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_kv_get(const char *key, void *buffer, uint32_t *buffer_len);

/**************************************************
 * @fn 		 hal_system_kv_set
 *
 * @brief     key-value设置函数
 *
 * @param     key: key-value的key指针
 *            value: 待设置成value的buffer
 *            len: value 的长度
 *            sync：是否进行同步写入flash操作
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_kv_set(const char *key, const void *value, int len,
                          int sync);

/**************************************************
 * @fn 		 hal_system_wifi_connect
 *
 * @brief     连接wifi
 *
 * @param     ssid：wifi名称，32位的字符串
 *            password: wifi密码,64位的字符串
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/

int32_t hal_system_wifi_connect(uint8_t ssid[32], uint8_t password[64]);

/**************************************************
 * @fn 		 hal_system_wifi_ssid_get
 *
 * @brief     获取wifi名称
 *
 * @param     ssid : buffer
 *            len: buffer len
 *
 * @return 	 成功返回0 失败返回-1
 **************************************************/
int32_t hal_system_wifi_ssid_get(char *ssid, int len);

int32_t hal_system_wifi_mac_get(uint8_t mac[6]);

/**************************************************
 * @fn 		 hal_system_wifi_init
 *
 * @brief     wifi初始化
 *
 * @param     无
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_wifi_init(void);

/**************************************************
 * @fn 		 hal_system_fs_init
 *
 * @brief     文件系统初始化，可以在此根据平台初始化spiffs或者fatfs
 *
 * @param     无
 *
 * @return 	 成功返回０　失败返回其他
 **************************************************/
int32_t hal_system_fs_init(void);

/**************************************************
 * @fn 		 hal_system_reboot
 *
 * @brief     系统重启函数
 *
 * @param     无
 *
 * @return 	 无
 **************************************************/
void hal_system_reboot(void);

#if defined(__cplusplus)
}
#endif
#endif /* BE_SYSTEM_H */
