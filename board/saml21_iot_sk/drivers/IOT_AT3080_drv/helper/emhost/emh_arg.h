/**
 ******************************************************************************
 * @file    emh_arg.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   AT command arguments to enum type convert header file
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

#ifndef _EMH_ARG_H_
#define _EMH_ARG_H_

#include "mx_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup emhost */
/** @{*/

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

#define EMH_ARG_ERR 0xFF

/******************************************************************************
 *                                 Enumerations
 ******************************************************************************/

/** \addtogroup alicloud_sds */
/** @{*/

enum{
	EMH_ARG_ALISDS_CONN_DISCONNECTED,
	EMH_ARG_ALISDS_CONN_CONNECTED,
	EMH_ARG_ALISDS_CONN_MAX,
};
typedef uint8_t emh_arg_alisds_conn_t;
extern const char* EMH_ARG_ALISDS_CONN[];

enum{
	EMH_ARG_ALISDS_STATUS_UNINITIALIZED,
	EMH_ARG_ALISDS_STATUS_INITIALIZED,
	EMH_ARG_ALISDS_STATUS_CONNECTED,
	EMH_ARG_ALISDS_STATUS_DISCONNECTED,
	EMH_ARG_ALISDS_STATUS_MAX,
};
typedef uint8_t emh_arg_alisds_status_t;
extern const char* EMH_ARG_ALISDS_STATUS[];

enum{
	EMH_ARG_ALISDS_FORMAT_JSON,
	EMH_ARG_ALISDS_FORMAT_RAW,
	EMH_ARG_ALISDS_FORMAT_MAX,
};
typedef uint8_t emh_arg_alisds_format_t;
extern const char* EMH_ARG_ALISDS_FORMAT[];

enum{
	EMH_ARG_ALISDS_EV_ALINK,
	EMH_ARG_ALISDS_EV_SET,
	EMH_ARG_ALISDS_EV_GET,
	EMH_ARG_ALISDS_EV_MAX,
};
typedef uint8_t emh_arg_alisds_ev_t;
extern const char* EMH_ARG_ALISDS_EV[];

/** @}*/
/** \addtogroup wlan */
/** @{*/
enum{
	EMH_ARG_WLAN_EV_UAP_ESTABLISHED,
	EMH_ARG_WLAN_EV_UAP_DESTROYED,
	EMH_ARG_WLAN_EV_STA_CONNECTED,
	EMH_ARG_WLAN_EV_STA_DISCONNECTED,
	EMH_ARG_WLAN_EV_MAX,
};
typedef uint8_t emh_arg_wlan_ev_t;
extern const char* EMH_ARG_WLAN_EV[];

enum{
	EMH_ARG_WLAN_STA_CONNECTED,
	EMH_ARG_WLAN_STA_DISCONNECTED,
	EMH_ARG_WLAN_STA_CONNECTING,
	EMH_ARG_WLAN_STA_MAX,
};
typedef uint8_t emh_arg_wlan_sta_t;      /**< wlan connection status under station mode */
extern const char* EMH_ARG_WLAN_STA[];
/** @}*/

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

/**
 * @brief 			Find string type argument from enum 
 * 
 * @param[in] 		emh_arg: Arg convert list array
 * @param[in] 		type: enum argument type
 *
 * @return 			Argument string type
 */
const char* emh_arg_for_type(const char* emh_arg[], uint8_t type);

/**
 * @brief 			Find enum type argument from string 
 * 
 * @param[in] 		emh_arg: Arg convert list array
 * @param[in] 		type: string argument read from AT command
 *
 * @return 			Argument enum type
 */
uint8_t emh_arg_for_arg(const char* emh_arg[], char *arg);

/** @}*/
/** @}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif