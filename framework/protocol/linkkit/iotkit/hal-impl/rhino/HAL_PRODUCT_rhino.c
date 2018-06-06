#include <stdio.h>
#include <string.h>
#include "iot_import.h"
#include "iot_import_product.h"
#include "iot_import_awss.h"
//#define PID_STR_MAXLEN              (64)
//#define MID_STR_MAXLEN              (64)
//#define PRODUCT_KEY_MAXLEN          (20)
//#define DEVICE_NAME_MAXLEN          (32)
//#define DEVICE_ID_MAXLEN            (64)
//#define DEVICE_SECRET_MAXLEN        (64)
//#define PRODUCT_SECRET_MAXLEN       (64)
//#define FIRMWARE_VERSION_MAXLEN     (32)

/* <TODO> */
#define PARTNER_ID "AliOSThings"
#define MODULE_ID "GeneralID"
#define DEVICE_ID "AosAwssDeviceID"
#define FW_VERSION "AosAwssFwVersion"
#define CHIP_ID "AosAwssChiID"



/**
 * @brief   获取设备的`Partner ID`, 仅用于紧密合作伙伴
 *
 * @param   pid_str : 用来存放Partner ID字符串的数组
 * @return  写到pid_str[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetPartnerID(_OU_ char pid_str[PID_STR_MAXLEN])
{
    int len = strlen(PARTNER_ID);
    strncpy(pid_str, PARTNER_ID, len);
    return len;
}


/**
 * @brief   获取设备的`Module ID`, 仅用于紧密合作伙伴
 *
 * @param   mid_str : 用来存放Module ID字符串的数组
 * @return  写到mid_str[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetModuleID(_OU_ char mid_str[MID_STR_MAXLEN])
{
    int len = strlen(MODULE_ID);
    strncpy(mid_str, MODULE_ID, len);
    return len;
}

/**
 * @brief   获取设备的`ProductKey`, 用于标识设备的品类, 三元组之一
 *
 * @param   product_key : 用来存放ProductKey字符串的数组
 * @return  写到product_key[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetProductKey(_OU_ char product_key[PRODUCT_KEY_MAXLEN])
{
    int len = strlen(PRODUCT_KEY);
    strncpy(product_key, PRODUCT_KEY, len);
    return len;
}

/**
 * @brief   获取设备的`DeviceName`, 用于标识设备单品的名字, 三元组之一
 *
 * @param   device_name : 用来存放DeviceName字符串的数组
 * @return  写到device_name[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceName(_OU_ char device_name[DEVICE_NAME_MAXLEN])
{
    int len = strlen(DEVICE_NAME);
    strncpy(device_name, DEVICE_NAME, len);
    return len;
}

/**
 * @brief   获取设备的`DeviceSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   device_secret : 用来存放DeviceSecret字符串的数组
 * @return  写到device_secret[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceSecret(_OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    int len = 0;
	
#ifdef SUPPORT_PRODUCT_SECRET
    len = DEVICE_SECRET_MAXLEN-1;
    if (0 != aos_kv_get("linkkit", device_secret, &len)) {
        return -1;
    }
#else
    len = strlen(DEVICE_SECRET);
    strncpy(device_secret, DEVICE_SECRET, len);
#endif
    return len;
}

/**
 * @brief   设置设备的`DeviceSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   device_secret : DeviceSecret字符串的数组
 * @return  device_secret[]数组中的字符长度, 单位是字节(Byte)
 */
#ifdef SUPPORT_PRODUCT_SECRET
int HAL_SetDeviceSecret(const char device_secret[DEVICE_SECRET_MAXLEN])
{
    if (!device_secret) {
        return -1;
    }

     if(strlen(device_secret) >= DEVICE_SECRET_MAXLEN) {
        return -1;
    }

    return aos_kv_set("linkkit", device_secret, strlen(device_secret)+1, 1);
}
#endif

/**
 * @brief   获取设备的`ProductSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   product_secret : 用来存放ProductSecret字符串的数组
 * @return  写到product_secret[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetProductSecret(_OU_ char product_secret[DEVICE_SECRET_MAXLEN])
{
    int len = strlen(PRODUCT_SECRET);
    strncpy(product_secret, PRODUCT_SECRET, len);
    return len;
}

/**
 * @brief   获取设备的`DeviceID`, 用于标识设备单品的ID
 *
 * @param   device_id : 用来存放DeviceID字符串的数组
 * @return  写到device_id[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceID(_OU_ char device_id[DEVICE_ID_MAXLEN])
{
    int len = strlen(DEVICE_ID);
    strncpy(device_id, DEVICE_ID, len);
    return len;
}

/**
 * @brief   获取设备的固件版本字符串
 *
 * @param   version : 用来存放版本字符串的数组
 * @return  写到version[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetFirmwareVesion(_OU_ char version[FIRMWARE_VERSION_MAXLEN])
{
    int len = strlen(FW_VERSION);
    strncpy(version, FW_VERSION, len);
    return len;
}

#define HAL_CID_LEN (64 + 1)
/**
 * @brief   获取唯一的芯片ID字符串
 *
 * @param   cid_str : 存放芯片ID字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN])
{
    int len = strlen(CHIP_ID);
    strncpy(cid_str, CHIP_ID, len);
    return NULL;
}

static hal_wireless_info_t hal_wireless_info = {
    .band = 0,
    .channel = 1,
    .rssi = -30,
    .snr = 30,
    .mac = {0x18, 0xFE, 0x34, 0x12, 0x34, 0x56},
    .tx_rate = 1,
    .rx_rate = 1,
};

int HAL_GetWirelessInfo(_OU_ hal_wireless_info_t *wireless_info)
{
    if (wireless_info) {
        memcpy(wireless_info, &hal_wireless_info, sizeof(hal_wireless_info_t));
    }

    return 0;
}

