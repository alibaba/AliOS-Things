/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IMPORT_PRODUCT_H__
#define __IMPORT_PRODUCT_H__

#define MAC_ADDR_LEN_MAX            (10)
#define PID_STRLEN_MAX              (64)
#define MID_STRLEN_MAX              (64)
#define IOTX_URI_MAX_LEN            (135)  /* IoTx CoAP/HTTP uri & MQTT topic maximal length */
#define PID_STR_MAXLEN              (64)
#define MID_STR_MAXLEN              (64)
#define PRODUCT_KEY_MAXLEN          (20 + 1)
#define DEVICE_NAME_MAXLEN          (32 + 1)
#define DEVICE_ID_MAXLEN            (64 + 1)
#define DEVICE_SECRET_MAXLEN        (64 + 1)
#define PRODUCT_SECRET_MAXLEN       (64 + 1)
#define FIRMWARE_VERSION_MAXLEN     (32 + 1)
#define HAL_CID_LEN                 (64 + 1)

/**
 * @brief   获取设备的固件版本字符串
 *
 * @param   version : 用来存放版本字符串的数组
 * @return  写到version[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetFirmwareVersion(_OU_ char version[FIRMWARE_VERSION_MAXLEN]);

/**
 * @brief   获取设备的`Partner ID`, 仅用于紧密合作伙伴
 *
 * @param   pid_str : 用来存放Partner ID字符串的数组
 * @return  写到pid_str[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetPartnerID(_OU_ char pid_str[PID_STR_MAXLEN]);

/**
 * @brief   获取设备的`Module ID`, 仅用于紧密合作伙伴
 *
 * @param   mid_str : 用来存放Module ID字符串的数组
 * @return  写到mid_str[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetModuleID(_OU_ char mid_str[MID_STR_MAXLEN]);


/**
 * @brief   获取设备的`DeviceID`, 用于标识设备单品的ID
 *
 * @param   device_id : 用来存放DeviceID字符串的数组
 * @return  写到device_id[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetDeviceID(_OU_ char device_id[DEVICE_ID_LEN]);

/**
 * @brief   获取唯一的芯片ID字符串
 *
 * @param   cid_str : 存放芯片ID字符串的缓冲区数组
 * @return  指向缓冲区数组的起始地址
 */
char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN]);

/**
 * @brief   设置设备的`ProductKey`, 用于标识设备的品类, 三元组之一
 *
 * @param   product_key : 用来存放ProductKey字符串的数组
 * @return  写到product_key[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_SetProductKey(_IN_ char *product_key);

/**
 * @brief   设置设备的`DeviceName`, 用于标识设备单品的名字, 三元组之一
 *
 * @param   device_name : 用来存放DeviceName字符串的数组
 * @return  写到device_name[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_SetDeviceName(_IN_ char *device_name);

/**
 * @brief   设置设备的`DeviceSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   device_secret : 用来存放DeviceSecret字符串的数组
 * @return  写到device_secret[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_SetDeviceSecret(_IN_ char *device_secret);

/**
 * @brief   设置设备的`ProductSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   product_secret : 用来存放ProductSecret字符串的数组
 * @return  写到product_secret[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_SetProductSecret(_IN_ char *product_secret);

/**
 * @brief   获取设备的`ProductKey`, 用于标识设备的品类, 三元组之一
 *
 * @param   product_key : 用来存放ProductKey字符串的数组
 * @return  写到product_key[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetProductKey(_OU_ char product_key[PRODUCT_KEY_LEN]);

/**
 * @brief   获取设备的`DeviceName`, 用于标识设备单品的名字, 三元组之一
 *
 * @param   device_name : 用来存放DeviceName字符串的数组
 * @return  写到device_name[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetDeviceName(_OU_ char device_name[DEVICE_NAME_LEN]);

/**
 * @brief   获取设备的`DeviceSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   device_secret : 用来存放DeviceSecret字符串的数组
 * @return  写到device_secret[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetDeviceSecret(_OU_ char device_secret[DEVICE_SECRET_LEN]);

/**
 * @brief   获取设备的`ProductSecret`, 用于标识设备单品的密钥, 三元组之一
 *
 * @param   product_secret : 用来存放ProductSecret字符串的数组
 * @return  写到product_secret[]数组中的字符长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_GetProductSecret(_OU_ char product_secret[DEVICE_SECRET_LEN]);

#define NIF_STRLEN_MAX (160)

/**
 ** @brief Retrieves all the info of the current network interfaces,
 ** including eth Mac, WiFi Mac, and IMEI/ICCID/IMSI/MSISDN for cellular connections.
 ** Note that the network interface length MUST NOT exceed NIF_STRLEN_MAX
 **
 ** @param [nif_str] give buffer to save network interface
 ** @return the lenth of the nif_str info
 ** @see None.
 ** @note None.
 **/
DLL_HAL_API int HAL_GetNetifInfo(char *nif_str);

#endif  /* __IMPORT_PRODUCT_H__ */
