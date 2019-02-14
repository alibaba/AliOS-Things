/**
 ****************************************************************************************
 *
 * @file AIS_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _AIS_TASK_H_
#define _AIS_TASK_H_


#include "rwprf_config.h"
#if  1// (BLE_FEB3_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
#include "ble_compiler.h"
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FEB3 Server task instances
#define FEB3S_IDX_MAX     0x01
///Maximal number of FFF0 that can be added in the DB

#define  FEB3_CHAR_DATA_LEN  20

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FEB3S task
enum feb3s_state
{
    /// Idle state
    FEB3S_IDLE,
    /// busy state
    FEB3S_BUSY,
    /// Number of defined states.
    FEB3S_STATE_MAX
};

/// Messages for FFF0 Server
enum feb3s_msg_id
{
	/// Start the FFE0 Server - at connection used to restore bond data
	FEB3S_ENABLE_REQ  =  TASK_FIRST_MSG(TASK_ID_FEB3S),

	/// Confirmation of the FEB3S Server start
	FEB3S_ENABLE_RSP,
	/// FED6 Level Value IND UPDATE Request
	FEB3S_FED6_VALUE_IND_UPD_REQ,
	/// Inform APP if FED6 Level value has been ind or not
	FEB3S_FED6_VALUE_IND_UPD_RSP,
	
	FEB3S_FED8_VALUE_NTF_UPD_REQ,
	/// Inform APP if FED8 Level value has been ntf or not
	FEB3S_FED8_VALUE_IND_UPD_RSP,
	
	/// Inform APP that FFE1 Level Notification Configuration has been changed - use to update bond data
	FEB3S_FED6_VALUE_IND_CFG_IND,
	
	FEB3S_FED8_VALUE_NTF_CFG_IND,

	FEB3S_FED5_WRITER_REQ_IND,
	
	FEB3S_FED7_WRITER_CMD_IND,

	FEB3S_FED4_READ_REQ_IND,
	FEB3S_FED4_READ_RSP,
	
	FEB3S_TEST_TX_SPEED_TIMER,
//	FEB3S_FEE8_WRITER_CMD_IND,

	FEB3S_START_AUTH_TIME,
	FEB3S_INTERACTIVE_TIME_OUT,
	FEB3S_AUTH_TIME_OUT,
	FEB3S_OTA_TIME_OUT,
	FEB3S_GPIO_ISR_TIME,
	FEB3S_GATTC_CMP_EVT,
	FEB3S_OTA_CMP_REBOOT_TIMER
};

/// Features Flag Masks
enum feb3s_features
{
    /// FED6 VALUE Characteristic doesn't support notifications
    FEB3S_FED6_FED8_VAL_IND_NTF_NOT_SUP = 0,
	  FEB3S_FED8_VAL_NTF_SUP  =  1 << 0,
		FEB3S_FED6_VAL_IND_SUP  =  1 << 1,
		
		/// FED8 VALUE Characteristic support notifications	 
    
    
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct feb3s_db_cfg
{
    /// Number of FFF0 to add
	uint8_t feb3s_nb;

	uint16_t cfg_flag;
	/// Features of each FEB3 instance
	uint8_t features;
};

/// Parameters of the @ref FEB3S_ENABLE_REQ message
struct feb3s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint16_t  ind_cfg;		
  	uint16_t  ntf_cfg;
    
};


/// Parameters of the @ref FEB3S_ENABLE_RSP message
struct feb3s_enable_rsp
{
    /// connection index
    uint8_t conidx;
    ///status
    uint8_t status;
};

///Parameters of the @ref FEB3S_FED6_VALUE_IND_UPD_REQ message
struct feb3s_fed6_value_ind_upd_req
{
    ///  instance
    uint8_t conidx;
	
	  uint16_t length;
	
  	uint16_t seq_num;
    /// ffe1 Level
    uint8_t value[__ARRAY_EMPTY];
};

///Parameters of the @ref FEB3S_FED6_VALUE_IND_UPD_RSP message
struct feb3s_fed6_value_ind_upd_rsp
{
	  uint8_t conidx;
	
  	uint16_t handle;
    ///status
    uint8_t status;
};
///Parameters of the @ref FEB3S_FED6_VALUE_IND_CFG_IND message
struct feb3s_fed6_value_ind_cfg_ind
{
    /// connection index
    uint8_t  conidx;
		uint16_t handle;
    ///Notification Configuration
    uint16_t  ind_cfg;
};



///Parameters of the @ref FEB3S_FED8_VALUE_NTF_UPD_REQ message
struct feb3s_fed8_value_ntf_upd_req
{
    ///  instance
    uint8_t conidx;
	
	  uint16_t length;
	
  	uint16_t seq_num;
    /// ffe1 Level
    uint8_t value[__ARRAY_EMPTY];
};

///Parameters of the @ref FEB3S_FED8_VALUE_NTF_UPD_RSP message
struct feb3s_fed8_value_ntf_upd_rsp
{
	  uint8_t conidx;
	
		uint16_t handle;
    ///status
    uint8_t status;
};





///Parameters of the @ref FEB3S_FED8_VALUE_NTF_CFG_IND message
struct feb3s_fed8_value_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
		uint16_t handle;
    ///Notification Configuration
    uint16_t  ntf_cfg;
};




/// Parameters of the @ref FEB3S_FED5_WRITER_REQ_IND message
struct feb3s_fed5_writer_req_ind
{
   
    uint8_t  conidx; /// Connection index
		uint16_t handle;
		uint16_t length;
	  uint8_t value[__ARRAY_EMPTY];
};

/// Parameters of the @ref FEB3S_FED5_WRITER_CMD_IND message
struct feb3s_fed7_writer_cmd_ind
{
   
    uint8_t conidx; /// Connection index
		uint16_t handle;
		uint16_t length;
	  uint8_t value[__ARRAY_EMPTY];
};


struct feb3s_fed4_read_req_ind
{
   
  uint8_t conidx; /// Connection index
	uint16_t handle;
	uint16_t rsp_id;	
};

struct feb3s_fed4_read_rsp
{
   
  uint8_t conidx; /// Connection index
	uint16_t length;
	uint16_t handle;
	uint16_t rsp_id;
	uint8_t value[__ARRAY_EMPTY];
};
	

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct kernel_state_handler feb3s_default_handler;
#endif // BLE_FEB3_SERVER


#endif /* _AIS_TASK_H_ */

