/*
 * airkiss.h
 *
 *  Created on: 2015-1-26
 *      Author: peterfan
 */

#ifndef AIRKISS_H_
#define AIRKISS_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef void* (*airkiss_memset_fn) (void* ptr, int value, unsigned int num);
typedef void* (*airkiss_memcpy_fn) (void* dst, const void* src, unsigned int num);
typedef int (*airkiss_memcmp_fn) (const void* ptr1, const void* ptr2, unsigned int num);
typedef int (*airkiss_printf_fn) (const char* format, ...);



typedef struct
{
	airkiss_memset_fn memset;
	airkiss_memcpy_fn memcpy;
	airkiss_memcmp_fn memcmp;
	airkiss_printf_fn printf;

} airkiss_config_t;

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup AirKiss_APIs AirKiss APIs
  * @brief AirKiss APIs
  *
  * API airkiss_lan_recv and airkiss_lan_pack are provided for the function that AirKiss can detect 
  * the ESP8266 devices in LAN, more details about AirKiss please refer to WeChat : http://iot.weixin.qq.com.
  *
  * Workflow : Create a UDP transmission. When UDP data is received, call API airkiss_lan_recv and 
  * input the UDP data, if the airkiss_lan_recv returns AIRKISS_LAN_SSDP_REQ, airkiss_lan_pack 
  * can be called to make a response packet.
  *
  */

/** @addtogroup AirKiss_APIs
  * @{
  */

/**
  * @brief       Get the version information of AirKiss lib.
  *
  * @attention The lenth of version is unknown
  *
  * @param     null.
  *
  * @return     the version information of AirKiss lib
  */

const char* airkiss_version(void);


typedef enum
{
	/** the length of the data buffer is lack*/
	AIRKISS_LAN_ERR_OVERFLOW = -5,

	/** Do not support the type of instruction */
	AIRKISS_LAN_ERR_CMD = -4,

	/** Error reading data package */
	AIRKISS_LAN_ERR_PAKE = -3,

	/** Error function passing parameters */
	AIRKISS_LAN_ERR_PARA = -2,

	/** Packet data error */
	AIRKISS_LAN_ERR_PKG = -1,

	/** Message format is correct */
	AIRKISS_LAN_CONTINUE = 0,

	/** Find equipment request packet is received */
	AIRKISS_LAN_SSDP_REQ = 1,

	/** Packet packaging complete */
	AIRKISS_LAN_PAKE_READY = 2


} airkiss_lan_ret_t;


typedef enum
{
	AIRKISS_LAN_SSDP_REQ_CMD = 0x1,
	AIRKISS_LAN_SSDP_RESP_CMD = 0x1001,
	AIRKISS_LAN_SSDP_NOTIFY_CMD = 0x1002
} airkiss_lan_cmdid_t;

/**
  * @brief     Parse the UDP packet sent by AirKiss.
  *
  * @param     const void* body : the start of the UDP message body data pointer.
  * @param     unsigned short length : the effective length of data.
  * @param     const airkiss_config_t* config : input struct airkiss_config_t
  *
  * @return    >=0 : succeed (reference airkiss_lan_ret_t)
  * @return    <0  : error code (reference airkiss_lan_ret_t)
  */

int airkiss_lan_recv(const void* body, unsigned short length, const airkiss_config_t* config);


/**
  * @brief     Packaging the UDP packet.
  *
  * @param     airkiss_lan_cmdid_t ak_lan_cmdid : type of the packet.
  * @param     void* appid       : Vendor's Wechat public number id, got from WeChat.
  * @param     void* deviceid   : device model id, got from WeChat.
  * @param     void* _datain    : user data waiting for packet assembly.
  * @param     unsigned short inlength  : the lenth of user data.
  * @param     void* _dataout              : data buffer addr, to store the packet got by _datain packet assembly.
  * @param     unsigned short* outlength        : the size of data buffer.
  * @param     const airkiss_config_t* config   : input struct airkiss_config_t
  *
  * @return    >=0 : succeed (reference airkiss_lan_ret_t)
  * @return    <0  : error code (reference airkiss_lan_ret_t)
  */

int airkiss_lan_pack(airkiss_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const airkiss_config_t* config);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* AIRKISS_H_ */
