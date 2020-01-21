/**
 ******************************************************************************
 * @file    emh_arg.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   Emhost SDK aAPIs
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef _EMH_API_H_
#define _EMH_API_H_

#include "mx_common.h"
#include "mx_debug.h"
#include "emh_arg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup emhost */
/** @{*/

/******************************************************************************
 *                             Module controller
 ******************************************************************************/

/** \addtogroup module */
/** @{*/

/**
 * @brief	Emhost SDK initialize and EMW module initialize
 * 
 * @return	status
 */
mx_status	emh_init				(void);

/**
 * @brief	Emhost SDK initialize and EMW module initialize
 * 
 * @return	none
 */
void		emh_runloop				(void);

/**
 * @brief	Software reset EMW module
 * 
 * @return	status
 */
mx_status	ewh_module_reset			(void);

/**
 * @brief	Read EMW module firmware version
 * 
 * @return	Point to firmware version string
 */
const char* emh_module_get_fw_version	(void);

/**
 * @brief	Read EMW module internal tick
 * 
 * @return	Tick value
 */
uint32_t    emh_module_get_tick			(void);

/**
 * @brief	Read EMW module internal tick
 * 
 * @return	status
 */
mx_status	emh_module_restore_settings	(void);

/**
 * @brief	Receive an unknown event, or data format error
 * 
 * @return	none
 */
MX_WEAK void emh_ev_unknown(void);

/** @}*/
/******************************************************************************
 *                              Wlan management
 ******************************************************************************/

/** \addtogroup wlan */
/** @{*/

/**
 * @brief		Read current configured AP's SSID and Password
 * 
 * @param[out] 	timeout: AT receive timeout, also changable by  #ATCmdParser_set_timeout
 * @param[out]	debug: Enable debug mode or not
 * 
 * @return	status
 */
mx_status			emh_wlan_get_para			(char ssid[33], char pwd[33]);

/**
 * @brief		Read wlan connection status
 * 
 * @return		Refer to #emh_arg_wlan_sta_t
 */
emh_arg_wlan_sta_t 	emh_wlan_get_sta_status		(void);

/**
 * @brief		Event: wlan connection status is changed
 * 
 * @return		none
 */
MX_WEAK void emh_ev_wlan(emh_arg_wlan_ev_t event);

/** @}*/
/******************************************************************************
 *                              Alicoud SDS service
 ******************************************************************************/

/** \addtogroup alicloud_sds */
/** @{*/

/**
 *  Alicloud sds service product information registered on cloud console
 */
typedef struct{
	struct{
		const char*				name;	/**< Reference to product TRD document */
		const char*				module;	/**< Reference to product TRD document */
		const char*				key;	/**< Reference to product TRD document */
		const char*				secret;	/**< Reference to product TRD document */
		emh_arg_alisds_format_t	format;	/**< Reference to product TRD document */
	} product_info;
	struct{
		const char*				type;			/**< Reference to product TRD document */
		const char*				category;		/**< Reference to product TRD document */
		const char*				manufacture;	/**< Reference to product TRD document */
	} dev_info ;
} emh_alisds_config_t;

/**
 *  Alicloud sds service message
 */
typedef struct {
	int32_t 	len;				/**< message length */
	uint8_t* 	data;				/**< point to the buffer store the message */
	emh_arg_alisds_format_t format;	/**< message format, json or raw data */
} emh_alisds_msg;

/**
 * @brief		Read product info data from module, write new data if not equal
 * 
 * @param[in] 	config: Product information
 * 
 * @return		status
 */
mx_status	emh_alisds_config(const emh_alisds_config_t *config);

/**
 * @brief		Start cloud service on module
 * 
 * @return		status
 */
mx_status emh_alisds_start_service(void);

/**
 * @brief		Start or stop AWS Wi-Fi configuration, cloud provision procedure bond module to APP
 * @warning		After bound device to APP, the device key can no longer used in other SDS products         
 * 
 * @param[in] 	on: true for start and false for stop 
 * 
 * @return		status
 */
mx_status emh_alisds_provision(bool on);

/**
 * @brief		Get current alicloud service connection status, 
 * 
 * @return		connection status
 */
emh_arg_alisds_status_t emh_alisds_get_status(void);

/**
 * @brief		Send message to cloud from local
 * 
 * @param[in] 	format: message format, json or raw data
 * @param[in] 	data: point to the buffer store the outgoing message
 * @param[in] 	len: outgoing message length
 * 
 * @return		status
 */
mx_status emh_alisds_set_cloud_atts	(emh_arg_alisds_format_t format, uint8_t *data, int32_t len);

/**
 * @brief		Set device key, device key is used to connect to SDS service.
 * @note  		The default key is stored in EMW module, use this function to write the new key.
 * 	            A factory restore command remove the new key and use the default key
 * 
 * @param[in] 	dev_key: device key
 * @param[in] 	dev_sec: device security
 * 
 * @return		status
 */
mx_status emh_alisds_set_key(const char *dev_key, const char *dev_sec);

/**
 * @brief		Un-bond device from APP, on works when connected to cloud
 * 
 * @return		status
 */
mx_status emh_alisds_unbound(void);

/**
 * @brief		Event: Alicloud SDS service connection status is changed
 * 
 * @return		none
 */
MX_WEAK void emh_ev_alisds_connection(emh_arg_alisds_conn_t conn);

/**
 * @brief		Event: Alicloud SDS service is requesting data from local device
 * 
 * @param[in] 	attrs: SDS service msg
 * 
 * @return		none
 */
MX_WEAK void emh_ev_alisds_get_local_atts(emh_alisds_msg *attrs);

/**
 * @brief		Event: Alicloud SDS service is writing data to local device
 * 
 * @param[in] 	attrs: SDS service msg
 * 
 * @return		none
 */
MX_WEAK void emh_ev_alisds_set_local_atts(emh_alisds_msg *attrs);

/** @}*/
/** @}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif