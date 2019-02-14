/**
 ****************************************************************************************
 *
 * @file AIS.h
 *
 * @brief Header file - FEB3 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _AIS_H_
#define _AIS_H_

/**
 ****************************************************************************************
 * @addtogroup  FEB3 'Profile' Server
 * @ingroup FEB3
 * @brief FEB3 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FEB3_SERVER)

#include "AIS_task.h"
#include "AIS_pub.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"
#include "ble_compiler.h"
/*
 * DEFINES
 ****************************************************************************************
 */
 
 
#define STR_MODEL_LEN   (20 + 1)    /**< Length of DEV ID */
#define STR_SEC_LEN     (40 + 1)    /**< Length of secret. */
#define STR_VER_LEN     (8 + 1)    /**< Length of version. */
#define DATA_RANDOM_LEN   (16 )    /**< Length of random. */


#define FEB3S_CFG_FLAG_MANDATORY_MASK       (0x1FFF)
#define FED6_CFG_FLAG_IND_SUP_MASK         (0x08)
#define FED8_CFG_FLAG_NTF_SUP_MASK         (0x08)

#define FED6_FLAG_IND_CFG_BIT             (0x03)
#define FED8_FLAG_NTF_CFG_BIT             (0x03)

/**************Service UUID  0xFEB3**************************************************************************************/
//***______________________________________________________________________________________________________________________
//***| Characteristics Name     		  |	Characteristics UUID   | Requirement | Properties  								  | Permission	|
//***| Read Characteristics     		  |	 0xFED4 							 |  Mandatory  |  Read       								  |  Read				|
//***| Write Characteristics    		  |  0xFED5 							 |  Mandatory  |  Read/Write 								  |  Write			|
//***| Indicate Characteristics 		  |	 0xFED6 							 |  Mandatory  |  Read/Indicate 							|  None				|
//***| WriteWithNoRsp Characteristics |  0xFED7 							 |  Option     |  Read/Write with No Response |  Write			|
//***| Notify Characteristics 			  |  0xFED8 			         |  Option     |  Read/Notify 								|  None				|
//-------------------------------------------------------------------------------------------------------------------------
/*************************************************************************************************************************/

enum
{		
	ATT_SVC_FEB3 			        = ATT_UUID_16(0xFEB3),

	ATT_CHAR_FEB3_FED4         = ATT_UUID_16(0xFED4),
	ATT_CHAR_FEB3_FED5         = ATT_UUID_16(0xFED5),
	ATT_CHAR_FEB3_FED6         = ATT_UUID_16(0xFED6),
	ATT_CHAR_FEB3_FED7         = ATT_UUID_16(0xFED7),
	ATT_CHAR_FEB3_FED8         = ATT_UUID_16(0xFED8),
};

/// FEB3S Service Attributes Indexes
enum
{
	FEB3S_IDX_SVC,	 
	FEB3S_IDX_FED4_VAL_CHAR,
	FEB3S_IDX_FED4_VAL_VALUE,
	
	FEB3S_IDX_FED5_VAL_CHAR,
	FEB3S_IDX_FED5_VAL_VALUE,
	
	
	FEB3S_IDX_FED7_VAL_CHAR,
	FEB3S_IDX_FED7_VAL_VALUE,
	
	FEB3S_IDX_FED6_VAL_CHAR,
	FEB3S_IDX_FED6_VAL_VALUE,
	FFB3S_IDX_FED6_VAL_IND_CFG,
	
	FEB3S_IDX_FED8_VAL_CHAR,
	FEB3S_IDX_FED8_VAL_VALUE,
	FFB3S_IDX_FED8_VAL_NTF_CFG,
	FEB3S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FEB3S 'Profile' Server environment variable
struct feb3s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct kernel_msg * operation;
    /// FEB3S Services Start Handle
    uint16_t start_hdl;
   
    /// BASS task state
    kernel_state_t state[FEB3S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint16_t ntf_cfg[BLE_CONNECTION_MAX];
	  ///indcation configuration of peer devices.
	  uint16_t ind_cfg[BLE_CONNECTION_MAX];
		
		uint8_t fed4_value[FEB3_CHAR_DATA_LEN];
		uint8_t fed5_value[FEB3_CHAR_DATA_LEN];
	  uint8_t fed6_value[FEB3_CHAR_DATA_LEN];
	  uint8_t fed7_value[FEB3_CHAR_DATA_LEN];
	  uint8_t fed8_value[FEB3_CHAR_DATA_LEN];
		uint8_t fed4_len;
		uint8_t fed5_len;
		uint8_t fed6_len;
		uint8_t fed7_len;
		uint8_t fed8_len;
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve feb3s service profile interface
 *
 * @return feb3s service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* feb3s_prf_itf_get(void);

uint16_t feb3s_get_att_handle(uint8_t att_idx);

uint8_t  feb3s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void feb3s_exe_operation(void);

#endif /* #if (BLE_FEB3_SERVER) */



#endif /*  _AIS_H_ */




