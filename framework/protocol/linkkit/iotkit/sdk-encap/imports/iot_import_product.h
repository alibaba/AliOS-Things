#ifndef __IMPORT_PRODUCT_H__
#define __IMPORT_PRODUCT_H__

#include "iot_import.h"

#define PID_STR_MAXLEN              (64)
#define MID_STR_MAXLEN              (64)
#define PRODUCT_KEY_MAXLEN          (20)
#define DEVICE_NAME_MAXLEN          (32)
#define DEVICE_ID_MAXLEN            (64)
#define DEVICE_SECRET_MAXLEN        (64)
#define FIRMWARE_VERSION_MAXLEN     (32)


#ifdef ON_DAILY
#define PRODUCT_KEY             "a1DPfXuJJsR"
#define PRODUCT_SECRET          "M4yY1ggbRnWYXzU3"
#define DEVICE_NAME             "test_3060_01"
#define DEVICE_SECRET           "6BhvTPQqCEj48gIHChFQKCWgDjNnQX5f"
#elif ON_PRE 
#define PRODUCT_KEY             "a1DQA90NlFe"
#define PRODUCT_SECRET          "0VDRjoQAbieMcYyZ"
#define DEVICE_NAME             "jaVam0S5dTq8XKUoI2NW"
#define DEVICE_SECRET           "IpGIaDsdUEpSvcOz237woypTlcxAV5RC"
#elif ON_PRE2
#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "light_mxchip_00"
#define DEVICE_SECRET           "T5DaOiKdR69BTOdEZduviliIsLQeM8Nn"
#else
#ifdef TEST_01
#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "test_light_01"
#define DEVICE_SECRET           "fxJ0cs1KUB4eM128GGeopbGl6LiacPJB"
#elif TEST_02
#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "test_light_02"
#define DEVICE_SECRET           "rOip6mfWD3sxO6pO9maUBTEJ1AjmK8xe"
#elif ESP8266
#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "ESP8266_cxf01"
#define DEVICE_SECRET           "K2pM9rSaQecgx8Vo31yjM6vmsXuQA8r9"
#elif  MQTT_TEST
#define PRODUCT_KEY             "yfTuLfBJTiL"
#define DEVICE_NAME             "TestDeviceForDemo"
#define DEVICE_SECRET           "fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c"
#define PRODUCT_SECRET          ""
#else
#define PRODUCT_KEY             "a1AzoSi5TMc"
#define PRODUCT_SECRET          "Z9Ze6qgMrWgTOezW"
#define DEVICE_NAME             "test_light_03"
#define DEVICE_SECRET           "oIdAOeech8fM7aHtq0QSvV1oSle30SxP"
#endif
#endif

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

#define HAL_CID_LEN (64 + 1)
/**
 * @brief   获取唯一的芯片ID字符串
 *
 * @param   cid_str : 存放芯片ID字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN]);

#endif  /* __IMPORT_PRODUCT_H__ */
