/**
 ******************************************************************************
 * @file    alicloud_sds.h
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   AliCloud SDS service functions and framework header file
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


#ifndef _ALICLOUD_SDS_H_
#define _ALICLOUD_SDS_H_

#include "emh_api.h"

/** \addtogroup protocols */
/** @{*/

/** \addtogroup Alicloud_SDS_Service */
/** @{*/

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

#define ALISDS_INVALID_HANDLE  (-1)			

/******************************************************************************
 *                                 Enumerations
 ******************************************************************************/

/** Alicloud sds service events */
enum alisds_event_e{
	ALISDS_EVENT_WLAN_CONFIG_STARTED,	/**< AWS service is started to set wlan and cloud. */
	ALISDS_EVENT_WLAN_CONNECTED,		/**< Device is connected to Wi-Fi access point. */
	ALISDS_EVENT_WLAN_DISCONNECTED,		/**< Device is disconnected from Wi-Fi access point. */
	ALISDS_EVENT_CLOUD_CONNECTED,		/**< Alicloud sds service is connected */
	ALISDS_EVENT_CLOUD_DISCONNECTED,	/**< Alicloud sds service is disconnected. */
};
typedef uint8_t alisds_event_t;	/**< Alicloud sds service events (see #alisds_event_e) */

/** Alicloud sds attribute types, protocol will convert values to SDS JSON packet according to these types */
enum alisds_att_type_e{
	ALI_ATT_TYPE_BOOL,		/**< Boolean value. convert to "0 or "1" */
	ALI_ATT_TYPE_INT,		/**< Integer value. convet to "123" */
	ALI_ATT_TYPE_FLOAT,		/**< Float value.  convet to "123.3" */
	ALI_ATT_TYPE_STRING,	/**< String value. no need to convert */
	ALI_ATT_TYPE_DATA,		/**< Raw data. convert to string with base64*/
	ALI_ATT_TYPE_NULL,
};
typedef uint8_t alisds_att_type_t;	/**< Alicloud sds attribute types (see #alisds_att_type_e) */

/******************************************************************************
 *                               Type Definitions
 ******************************************************************************/

typedef int alisds_attr_handle_t;

/** Alicloud sds attribute value */
typedef union {
	bool		boolValue;
	int			intValue;
	double		floatValue;
	char*		stringValue;
	char*		dateValue;
} alisds_att_val_t;

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

/**
 * Cloud read attribute handler
 *
 * @param value[out] : point to attribute value buffer, application should write the correct value
 *
 * @return status
 */
typedef mx_status (*alisds_read_attr)(alisds_att_val_t *value);

/**
 * Cloud write attribute handler
 *
 * @param[in] value : New attribute vale sent from the SDS cloud
 *
 * @return status
 */
typedef mx_status (*alisds_write_attr)(alisds_att_val_t value);

/** Alicloud sds attribute description */
typedef struct {
	char*					name;			/**< Attribute name */
	alisds_att_type_t		type;			/**< Attribute value type */
	alisds_read_attr		read_func;		/**< Attribute value read handler, optional if the value is readable*/
	alisds_write_attr		write_func;		/**< Attribute value write handler, optional if the value is writable */
} alisds_attr_t;


/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

/**
 * @brief	        Initialize EMW module with product info registered on cloud 
 *                  console, allocate memory for SDS attr handle database. 
 * 	                Should be called before attr initialize see #alisds_attr_init
 * 
 * @param[in]       config       : SDS product info registered on cloud console
 * @param[in]       num_handles  : The max. attribute numbers, required to allocate 
 *                                 memory for attr database
 *
 * @return      	status
 *
 */
mx_status alisds_init(const emh_alisds_config_t *config, int num_handles);


/**
 * @brief        	SDS Attribute initialization, add the attribute to database
 * 
 * @param[in]       handle	: attribute handle
 * @param[in]       attr 	: Alicloud sds attribute description
 * @return      	status
 *
 */
mx_status alisds_attr_init(alisds_attr_handle_t handle, alisds_attr_t attr);

/**
 *
 * @brief        	Alicould sds service runloop, application should called  
 *                  periodically to handle events and transfer data.
 *                  To save power, also can be called when uart data is ready 
 *                  to receive or sensor data is ready to send to cloud
 * @note            Never call this function in event handler
 * 
 * @return      	status
 *
 */
mx_status alisds_runloop(void);

/**
 *
 * @brief           Prepare to send attribute to cloud, attribute value will 
 *                  be read and send in SDS runloop alisds_runloop
 * @note            This function do not send data use AT command, so it can 
 * 	                be called inside the event handler. #alisds_runloop will 
 *                  do the actually read and send function.
 * 
 * @param[in]       handle	: attribute handle
 * @param[in]       attr 	: Alicloud sds attribute description
 * @return      	status
 *
 */
void alisds_attr_indicate_by_handle(int handle);

/**
 * 
 * @brief 	Send provision data to SDS cloud, this usually called after AWS 
 *          Wi-Fi config to prove APP is bound to a correct device. provision 
 *          data is defined on SDS cloud console.
 * 
 * @return	none
 *
 */
void alisds_provision(void);

/**
 *
 * @brief	Try to procedure an unbound operation on cloud if connected and 
 *          restore module settings to default.
 * 
 * @return	none
 *
 */
void alisds_restore(void);

/**
 *
 * @brief	Alicloud SDS service event handler accomplished by application 
 * 
 * @return	none
 *
 */
MX_WEAK void alisds_event_handler(alisds_event_t event);

#endif  //_ALICLOUD_SDS_H_

/** @}*/
/** @}*/