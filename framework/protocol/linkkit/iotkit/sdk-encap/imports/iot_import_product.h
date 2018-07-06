#ifndef __IMPORT_PRODUCT_H__
#define __IMPORT_PRODUCT_H__

#include "iot_import.h"

#define _IN_                                /* indicate that this is an input parameter */
#define _OU_                                /* indicate that this is an output parameter */
#define IOT_TRUE                    (1)     /* indicate boolean value true */
#define IOT_FALSE                   (0)     /* indicate boolean value false */
#define PID_STR_MAXLEN              (64 + 1)
#define MID_STR_MAXLEN              (64 + 1)
#define PRODUCT_KEY_MAXLEN          (20 + 1)
#define PRODUCT_SECRET_MAXLEN       (64 + 1)
#define DEVICE_NAME_MAXLEN          (32 + 1)
#define DEVICE_ID_MAXLEN            (64 + 1)
#define DEVICE_SECRET_MAXLEN        (64 + 1)
#define HAL_CID_LEN (64 + 1)
#define FIRMWARE_VERSION_MAXLEN     (32 + 1)
#define IOTX_URI_MAX_LEN         (135)  /* IoTx CoAP/HTTP URI & MQTT topic maximal length */


#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "light_alen_02"
#define DEVICE_SECRET           "gCV7RxK6yM7bLsA2vBjy2v6kL7WjY9n2"

/**
 * @brief   获取设备的`Partner ID`, 仅用于紧密合作伙伴
 *
 * @param   pid_str : 用来存放Partner ID字符串的数组
 * @return  写到pid_str[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetPartnerID(_OU_ char pid_str[PID_STR_MAXLEN]);

/**
 * @brief   获取设备的`Module ID`, 仅用于紧密合作伙伴
 *
 * @param   mid_str : 用来存放Module ID字符串的数组
 * @return  写到mid_str[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetModuleID(_OU_ char mid_str[MID_STR_MAXLEN]);

/**
 * @brief   获取设备的`ProductKey`, 用于标识设备的品类, 三元组之一
 *
 * @param   product_key : 用来存放ProductKey字符串的数组
 * @return  写到product_key[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetProductKey(_OU_ char product_key[PRODUCT_KEY_MAXLEN]);

/**
 * @brief   获取设备的`DeviceName`, 用于标识设备单品的名字, 三元组之一
 *
 * @param   device_name : 用来存放DeviceName字符串的数组
 * @return  写到device_name[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceName(_OU_ char device_name[DEVICE_NAME_MAXLEN]);

/**
 * @brief   获取设备的`DeviceSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   device_secret : 用来存放DeviceSecret字符串的数组
 * @return  写到device_secret[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceSecret(_OU_ char device_secret[DEVICE_SECRET_MAXLEN]);

/**
 * @brief   获取设备的`DeviceID`, 用于标识设备单品的ID
 *
 * @param   device_id : 用来存放DeviceID字符串的数组
 * @return  写到device_id[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetDeviceID(_OU_ char device_id[DEVICE_ID_MAXLEN]);

/**
 * @brief   获取设备的固件版本字符串
 *
 * @param   version : 用来存放版本字符串的数组
 * @return  写到version[]数组中的字符长度, 单位是字节(Byte)
 */
int HAL_GetFirmwareVesion(_OU_ char version[FIRMWARE_VERSION_MAXLEN]);

/**
 * @brief   获取唯一的芯片ID字符串
 *
 * @param   cid_str : 存放芯片ID字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN]);

typedef struct _hal_wireless_info_t {
    char mac[6];            /* parent'mac, such as Ap's bssid. */
    int rssi;               /* wireless RSSI, range of [-127 : -1]. */
    int band;               /*wireless band, 0: wifi 2.4G, 1: wifi 5G, ... */
    int snr;                /* wireless SNR. */
    int channel;            /* wireless channel. */
    float tx_rate;          /* 0.25 : 250Kbps, 1: 1Mbps，2: 2Mbps，5.5: 5.5Mbps，...*/
    float rx_rate;          /* 0.25 : 250Kbps, 1: 1Mbps，2: 2Mbps，5.5: 5.5Mbps，...*/
} hal_wireless_info_t;
/**
 * @brief   获取无线信号信息
 *
 * @param   wireless_info : 存放获取无线信号信息的缓冲区
 * @return  执行结果
 *   0:  success
 *   -1: failure
 */
int HAL_GetWirelessInfo(_OU_ hal_wireless_info_t *wireless_info);

#endif  /* __IMPORT_PRODUCT_H__ */
