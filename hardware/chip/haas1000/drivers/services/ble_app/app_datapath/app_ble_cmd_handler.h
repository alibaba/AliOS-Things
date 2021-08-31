/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __APP_BLE_CMD_HANDLER_H__
#define __APP_BLE_CMD_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_timer.h"

#define GET_CURRENT_TICKS()			hal_sys_timer_get()

#define GET_CURRENT_MS()			TICKS_TO_MS(GET_CURRENT_TICKS())

extern uint32_t __custom_handler_table_start[];
extern uint32_t __custom_handler_table_end[];

#define CUSTOM_COMMAND_TO_ADD(cmdCode, cmdHandler, isNeedResponse, timeoutWaitingRspInMs, cmdRspHandler)	\
	static const BLE_CUSTOM_CMD_INSTANCE_T cmdCode##_entry __attribute__((used, section(".custom_handler_table"))) = 	\
		{(cmdCode), (cmdHandler), (isNeedResponse), (timeoutWaitingRspInMs), (cmdRspHandler)};

#define CUSTOM_COMMAND_PTR_FROM_ENTRY_INDEX(index)	\
	((BLE_CUSTOM_CMD_INSTANCE_T *)((uint32_t)__custom_handler_table_start + (index)*sizeof(BLE_CUSTOM_CMD_INSTANCE_T)))

#define INVALID_CUSTOM_ENTRY_INDEX		0xFFFF

/**
 * @brief The BLE custom command handling return status
 *
 */
typedef enum
{
	NO_ERROR = 0,
	INVALID_CMD_CODE,
	PARAMETER_LENGTH_OUT_OF_RANGE,
	PARAMETER_LENGTH_TOO_SHORT,
	HANDLING_FAILED,	
	TIMEOUT_WAITING_RESPONSE,
	// TO ADD: new return status
} BLE_CUSTOM_CMD_RET_STATUS_E;

/**
 * @brief Type of the BLE data transmission path
 *
 */
typedef enum
{
	TRANSMISSION_VIA_NOTIFICATION = 0,
	TRANSMISSION_VIA_INDICATION,
	TRANSMISSION_VIA_WRITE_CMD,
	TRANSMISSION_VIA_WRITE_REQ,
} BLE_CUSTOM_CMD_TRANSMISSION_PATH_E;

/**
 * @brief Format of the custom command handler function, called when the command is received
 *
 * @param cmdCode	Custom command code, from BLE_CUSTOM_CMD_CODE_E
 * @param ptrParam 	Pointer of the received parameter
 * @param paramLen 	Length of the recevied parameter
 * 
 */
typedef void (*BLE_CustomCmdHandler_t)(uint32_t cmdCode, uint8_t* ptrParam, uint32_t paramLen);

/**
 * @brief Format of the custom command response handler function, 
 *	called when the response to formerly sent command is received
 *
 * @param retStatus	Handling return status of the command
 * @param ptrParam 	Pointer of the received parameter
 * @param paramLen 	Length of the recevied parameter
 * 
 */
typedef void (*BLE_CustomCmd_Response_Handler_t)(BLE_CUSTOM_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);

/**
 * @brief Format of the raw data xfer received handler function
 *
 * @param ptrData	Pointer of the received raw data
 * @param dataLen 	Length of the received raw data
 * 
 */
typedef void (*BLE_RawDataReceived_Handler_t)(uint8_t* ptrData, uint32_t dataLen);


/**
 * @brief Custom command definition data structure
 *
 */
typedef struct
{
	uint16_t				cmdCode;
    BLE_CustomCmdHandler_t 	cmdHandler;  			/**< command handler function */
    uint8_t 				isNeedResponse;     	/**< true if needs the response from the peer device */
	uint16_t				timeoutWaitingRspInMs;	/**< time-out of waiting for response in milli-seconds */
	BLE_CustomCmd_Response_Handler_t	cmdRspHandler;	/**< command response handler function */
} BLE_CUSTOM_CMD_INSTANCE_T;

/**< maximum payload size of one BLE custom command */
#define BLE_CUSTOM_CMD_MAXIMUM_PAYLOAD_SIZE		20	// assure that one BLE packet can include all the data 

/**
 * @brief BLE custom command playload
 *
 */
typedef struct
{
    uint16_t 	cmdCode;    	/**< command code, from BLE_CUSTOM_CMD_CODE_E */
	uint16_t 	paramLen;		/**< length of the following parameter */
    uint8_t 	param[BLE_CUSTOM_CMD_MAXIMUM_PAYLOAD_SIZE - 2*sizeof(uint16_t)];
} BLE_CUSTOM_CMD_PAYLOAD_T;

/**
 * @brief Command response parameter structure
 *
 */
typedef struct
{
    uint16_t 	cmdCodeToRsp;  	/**< tell which command code to response */
    uint16_t 	cmdRetStatus;   /**< handling result of the command, from BLE_CUSTOM_CMD_RET_STATUS_E */
	uint16_t 	rspDataLen;		/**< length of the response data */
    uint8_t 	rspData[BLE_CUSTOM_CMD_MAXIMUM_PAYLOAD_SIZE - 5*sizeof(uint16_t)];
} BLE_CUSTOM_CMD_RSP_T;

void 						BLE_get_response_handler(uint32_t funcCode, uint8_t* ptrParam, uint32_t paramLen);
void 						BLE_raw_data_xfer_control_handler(uint32_t funcCode, uint8_t* ptrParam, uint32_t paramLen);
void 						BLE_control_raw_data_xfer(bool isStartXfer);
void 						BLE_set_raw_data_xfer_received_callback(BLE_RawDataReceived_Handler_t callback);
void 						BLE_start_raw_data_xfer_control_rsp_handler(BLE_CUSTOM_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
void 						BLE_stop_raw_data_xfer_control_rsp_handler(BLE_CUSTOM_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
uint8_t* 					BLE_custom_command_raw_data_buffer_pointer(void);
uint16_t 					BLE_custom_command_received_raw_data_size(void);
BLE_CUSTOM_CMD_RET_STATUS_E BLE_send_response_to_command(uint32_t responsedCmdCode, BLE_CUSTOM_CMD_RET_STATUS_E returnStatus, 
								uint8_t* rspData, uint32_t rspDataLen, BLE_CUSTOM_CMD_TRANSMISSION_PATH_E path);
BLE_CUSTOM_CMD_RET_STATUS_E BLE_send_custom_command(uint32_t cmdCode, uint8_t* ptrParam, uint32_t paramLen, BLE_CUSTOM_CMD_TRANSMISSION_PATH_E path);
BLE_CUSTOM_CMD_RET_STATUS_E BLE_custom_command_receive_data(uint8_t* ptrData, uint32_t dataLength);
void 						BLE_custom_command_init(void);
BLE_CUSTOM_CMD_INSTANCE_T* 	BLE_custom_command_get_entry_pointer_from_cmd_code(uint16_t cmdCode);
uint16_t 					BLE_custom_command_get_entry_index_from_cmd_code(uint16_t cmdCode);



#ifdef __cplusplus
	}
#endif

#endif // #ifndef __APP_BLE_CMD_HANDLER_H__

